#include "aimbot.hpp"
#include "cheats.hpp"
#include "KmBox/KmboxNet.hpp"

int Aimbot::GetRandomInt(int minVal, int maxVal)
{
	HCRYPTPROV hProv = 0;
	DWORD randVal = 0;

	if (CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		if (CryptGenRandom(hProv, sizeof(randVal), reinterpret_cast<BYTE*>(&randVal))) {
			CryptReleaseContext(hProv, 0);
			return minVal + (randVal % (maxVal - minVal + 1));
		}
		CryptReleaseContext(hProv, 0);
	}

	static bool seeded = false;
	if (!seeded) {
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		seeded = true;
	}
	return minVal + (std::rand() % (maxVal - minVal + 1));
}

bool Aimbot::KmNetInit(char* ip, char* port, char* mac) {
	if (kmNet_init(ip, port, mac) == 0)
	{
		std::cout << "[+] Connected to KmboxNet." << std::endl;
		Global::deviceConnected = true;
	}
	else {
		std::cerr << "[!] Cannot connect to KmboxNet!" << std::endl;
		return false;
	}
	return true;
}

float Aimbot::CalcAimHeight(RectData rect, float AimPos, bool isKnocked)
{
	int Y_min = rect.Y;
	int Y_max = rect.Y + rect.H;
	if (isKnocked)
		rect.H /= 2.0f;
	float result = Y_max - rect.H * AimPos;

	if (result < 0)
		result = 0;
	else if (result > Global::ScreenSize.y)
		result = Global::ScreenSize.y;

	return result;
}

Vector2 Aimbot::CalcAimPos(RectData rect, bool isKnocked, int duckState)
{
	Vector2 aimPos;
	aimPos.x = rect.X;
	aimPos.y = CalcAimHeight(rect, 0.6f, isKnocked);
	if (!isKnocked && duckState > 0)
		aimPos.y /= 2;

	return aimPos;
}

Vector2 Aimbot::BestTarget(const std::vector<Vector2>& aimPoints)
{
	Vector2 bestTarget(-1, -1);
	float bestDist = 999999.0f;

	for (const auto& pt : aimPoints) {
		if (pt.x == -1 && pt.y == -1)
			continue;

		float dist = Distance2(Global::SightCenter.x, Global::SightCenter.y, pt.x, pt.y);
		if (dist < bestDist) {
			bestDist = dist;
			bestTarget = pt;
		}
	}

	return bestTarget;
}

void Aimbot::MoveMouse(float x, float y)
{
	static DWORD lastTick = 0;
	static int refreshRate = GetMonitorRefreshRate();

	DWORD currentTick = GetTickCount();
	int targetFPS = refreshRate;
	DWORD minInterval = 1000 / targetFPS;

	if (currentTick - lastTick >= minInterval)
	{
		kmNet_mouse_move((short)x, (short)y);
		lastTick = currentTick;
	}
}

void Aimbot::MouseAimbot(const std::vector<Vector2>& aimPoints, float smooth)
{
	Vector2 bestTarget = BestTarget(aimPoints);
	if (bestTarget.x == -1 && bestTarget.y == -1)
		return;

	float dist = Distance2(Global::SightCenter.x, Global::SightCenter.y,
		bestTarget.x, bestTarget.y);

	if (dist < cfg::aimbotFov) {
		float dx = (bestTarget.x - Global::SightCenter.x) / smooth;
		float dy = (bestTarget.y - Global::SightCenter.y) / smooth;
		MoveMouse(dx, dy);
	}
}

Vector3 Aimbot::BoneToScreen(uintptr_t entityPtr, Matrix m, int BoneID)
{
	Vector3 BonePos = NewHitbox(entityPtr, BoneID);
	Vector3 ScreenPos;
	world_to_screen(BonePos, m, Global::ScreenSize.x, Global::ScreenSize.y, ScreenPos);
	return ScreenPos;
}

void Aimbot::Run()
{
	std::vector<Player>* snapCopy = GlobalBuffer::frontBuffer.load();

	int react_delay_dist = GetRandomInt(8, 110);
	int curve_amp_dist = -0.06f + (GetRandomInt(0, 1000000) / 1000000.0f) * (0.12f);

	float remainX = 0.f;
	float remainY = 0.f;

	uint64_t lastLockedAddr = 0;

	bool wasTriggerHeld = false;
	auto triggerStartTime = std::chrono::steady_clock::now();
	int currentReactionDelay = 0;

	float initialDistance = 0.f;
	float curveAmplitude = 0.f;

	auto last_calc_time = std::chrono::steady_clock::now();
	auto last_mouse_time = std::chrono::steady_clock::now();

	int next_mouse_interval = (GetRandomInt(1, 100) <= 90) ? 1000 : GetRandomInt(990, 1047);

	while (Cheat::running)
	{
		auto frame_start = std::chrono::steady_clock::now();

		Player local = GlobalBuffer::localPlayerSnapshot;
		if (local.Ptr == 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			continue;
		}

		float dt = std::chrono::duration<float>(frame_start - last_calc_time).count();
		last_calc_time = frame_start;

		if (dt > 0.1f) dt = 0.1f;
		if (dt < 0.0001f) dt = 0.0001f;

		short k1 = GetAsyncKeyState(VK_LBUTTON);
		short k2 = GetAsyncKeyState(VK_LBUTTON);
		bool isTriggerHeld = (k1 & 0x8000) || (k2 & 0x8000);

		bool aimActive = false;

		if (cfg::aimbotEnabled)
		{
			aimActive = isTriggerHeld;
		}

		int zoomed = drv.RPM<int>(local.Ptr + bZooming);
		float currentFOV = (zoomed == 0) ? cfg::aimbotFov : cfg::aimbotFov_ADS;

		if (!aimActive) {
			remainX = 0.f; remainY = 0.f;
			wasTriggerHeld = false;
			lastLockedAddr = 0;
			initialDistance = 0.f;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		float exactMoveX = 0.f;
		float exactMoveY = 0.f;

		if (aimActive && !wasTriggerHeld) {
			wasTriggerHeld = true;
			triggerStartTime = std::chrono::steady_clock::now();
			currentReactionDelay = react_delay_dist;
			lastLockedAddr = 0;
			initialDistance = 0.f;
		}
		else if (!aimActive) {
			wasTriggerHeld = false;
			lastLockedAddr = 0;
			initialDistance = 0.f;
		}

		if (aimActive)
		{
			// printf("[DEBUG]Aimbot Actived\n");
			auto elapsedReact =
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - triggerStartTime).count();
			if (elapsedReact >= currentReactionDelay)
			{
				// printf("[DEBUG]Aimbot Calc\n");
				auto calcAimbot = [&]() -> bool
					{
						std::vector<TargetBoneInfo> activeBones;
						activeBones.push_back({ 0, 3.0f, 0.02f, 2 });
						activeBones.push_back({ 1, 2.0f, 0.02f, 2 });
						
						activeBones.push_back({ 2, 5.5f, 0.03f, 3 });
						activeBones.push_back({ 3, 5.5f, 0.03f, 3 });

						activeBones.push_back({ 4, 5.5f, 0.03f, 2 });
						activeBones.push_back({ 5, 6.0f, 0.03f, 2 });

						activeBones.push_back({ 6, 2.5f, 0.0f, 0 });
						activeBones.push_back({ 7, 2.0f, 0.0f, 0 });
						activeBones.push_back({ 9, 2.5f, 0.0f, 0 });
						activeBones.push_back({ 10, 2.0f, 0.0f, 0 });
						
						activeBones.push_back({ 12, 3.5f, 0.0f, 0 });
						activeBones.push_back({ 13, 3.0f, 0.0f, 0 });
						activeBones.push_back({ 16, 3.5f, 0.0f, 0 });
						activeBones.push_back({ 17, 3.0f, 0.0f, 0 });

						int localTeam = GetTeamID(local.Ptr);

						if (snapCopy->empty()) return false;

						const float cx = Global::SightCenter.x;
						const float cy = Global::SightCenter.y;

						const Player* bestTarget = nullptr;
						float bestDist2 = pow(currentFOV, 2);

						for (const auto& plyer : *snapCopy)
						{
							if (plyer.LifeState != 0 || plyer.TeamID == localTeam) continue;
							if (cfg::NotAimAtKnocked && plyer.IsKnocked) continue;

							Vector3 ptFeet, ptHead;
							bool visFeet = world_to_screen(plyer.Position, Global::ViewMatrix, Global::ScreenSize.x, Global::ScreenSize.y, ptFeet);
							bool visHead = world_to_screen(Vector3(plyer.Position.x, plyer.Position.y, plyer.Position.z + 72), Global::ViewMatrix, Global::ScreenSize.x, Global::ScreenSize.y, ptHead);

							if (visFeet || visHead)
							{
								float dFeetSq = visFeet ? ((ptFeet.x - cx) * (ptFeet.x - cx) + (ptFeet.y - cy) * (ptFeet.y - cy)) : 9999999.f;
								float dHeadSq = visHead ? ((ptHead.x - cx) * (ptHead.x - cx) + (ptHead.y - cy) * (ptHead.y - cy)) : 9999999.f;
								float minDist2 = min(dFeetSq, dHeadSq);

								if (minDist2 < bestDist2)
								{
									bestDist2 = minDist2;
									bestTarget = &plyer;
								}
							}
						}
						if (!bestTarget) return false;
						// printf("[DEBUG]Aimbot Found Target\n");

						uint64_t lockedAddr = bestTarget->Ptr;

						if (lockedAddr != lastLockedAddr) {
							lastLockedAddr = lockedAddr;
							initialDistance = 0.0f;
							curveAmplitude = curve_amp_dist;
						}

						uint64_t localPlayerPtr = local.Ptr;
						if (localPlayerPtr == 0) return false;


						Vector3 targetPos = GetPosition(lockedAddr);
						int targetKnocked = GetKnocked(lockedAddr);
						int targetLifestate = GetLifeState(lockedAddr);


						Matrix3x4 freshBones[19];
						for (const auto& bInfo : activeBones) {
							Vector3 bonePos = bestTarget->CachedBones[bInfo.hitboxId];

							freshBones[bInfo.hitboxId]._14 = bonePos.x;
							freshBones[bInfo.hitboxId]._24 = bonePos.y;
							freshBones[bInfo.hitboxId]._34 = bonePos.z;
						}

						if (local.LifeState != 0 || local.IsKnocked) return false;
						if (targetLifestate != 0) return false;
						if (cfg::NotAimAtKnocked && targetKnocked != 0) return false;
						if (!GetVisible(local.Ptr, lockedAddr)) return false;

						float targetBestDist2 = pow(currentFOV, 2) * 2.f;
						float dx = 0.f, dy = 0.f;
						bool foundTargetHitbox = false;
						Vector3 finalPredictPos;

						for (const auto& bInfo : activeBones)
						{
							Vector3 boneWorld = NewHitbox(bestTarget->Ptr, bInfo.hitboxId);

							if (boneWorld.x == targetPos.x && boneWorld.y == targetPos.y) continue;

							Vector3 predictPosition = boneWorld;

							Vector3 boneCenterScreen;
							if (world_to_screen(predictPosition, Global::ViewMatrix, cx * 2, cy * 2, boneCenterScreen))
							{
								Vector2 evalPoints[4];
								int evalCount = 0;
								evalPoints[evalCount++] = { boneCenterScreen.x, boneCenterScreen.y };
								if (bInfo.offsetCount > 0)
								{
									float safeRadius3D = bInfo.radiusUnit * bInfo.customRatio;
									Vector3 topScreen;
									float pixelRadius = 1.0f;

									if (world_to_screen(predictPosition + Vector3(0, 0, safeRadius3D), Global::ViewMatrix, cx * 2, cy * 2, topScreen))
									{
										pixelRadius = std::abs(boneCenterScreen.y - topScreen.y);
									}
									if (pixelRadius < 1.0f) pixelRadius = 1.0f;

									if (bInfo.offsetCount == 2)
									{
										evalPoints[evalCount++] = { boneCenterScreen.x - pixelRadius, boneCenterScreen.y };
										evalPoints[evalCount++] = { boneCenterScreen.x + pixelRadius, boneCenterScreen.y };
									}
									else if (bInfo.offsetCount == 3)
									{
										float r866 = pixelRadius * 0.866025f;
										float r500 = pixelRadius * 0.5f;
										evalPoints[evalCount++] = { boneCenterScreen.x, boneCenterScreen.y - pixelRadius };
										evalPoints[evalCount++] = { boneCenterScreen.x - r866, boneCenterScreen.y + r500 };
										evalPoints[evalCount++] = { boneCenterScreen.x + r866, boneCenterScreen.y + r500 };
									}

									for (int k = 0; k < evalCount; k++)
									{
										float ddx = evalPoints[k].x - cx;
										float ddy = evalPoints[k].y - cy;
										float d2 = pow(ddx, 2) + pow(ddy, 2);
										if (d2 < targetBestDist2)
										{
											targetBestDist2 = d2;
											dx = ddx;
											dy = ddy;
											foundTargetHitbox = true;
											finalPredictPos = predictPosition;
										}
									}
								}
							}
						}

						// printf("[DEBUG]Aimbot Found Target Hitbox\n");
						if (!foundTargetHitbox) return false;

						// float targetDistMeters = localCameraPos.distance_to(finalPredictPos) * 0.0254f;

						float blendStart = 15.0f;
						float blendRange = (((1.0f) > (100 - blendStart)) ? (1.0f) : (100 - blendStart));
						// float tDist = std::clamp((targetDistMeters - blendStart) / blendRange, 0.0f, 1.0f);

						float smth = cfg::aimbotSmooth;
						float smthads = cfg::aimbotSmooth_zoomed;

						float currentSmooth = zoomed ? smthads : smth;

						float pixelDist = std::sqrt(dx * dx + dy * dy);

						if (initialDistance <= 0.0f || pixelDist > initialDistance)
						{
							initialDistance = pixelDist;
						}

						float t = 1.0f;
						if (initialDistance > 1.0f)
						{
							t = 1.0f - (pixelDist / initialDistance);
						}
						t = std::clamp(t, 0.0f, 1.0f);

						float bellCurve = std::sin(t * 3.1415926535f);

						float timeScale = dt * 60.0f;
						float realSmooth = std::clamp(currentSmooth * timeScale, 0.0001f, 1.0f);

						float perpX = -dy;
						float perpY = dx;
						float perpLen = std::sqrt(perpX * perpX + perpY * perpY);
						if (perpLen > 0.0001f)
						{
							perpX /= perpLen;
							perpY /= perpLen;
						}

						float curveAmount = bellCurve * curveAmplitude * initialDistance;

						float modDx = dx + perpX * curveAmount;
						float modDy = dy + perpY * curveAmount;

						exactMoveX += modDx * realSmooth;
						exactMoveY += modDy * realSmooth;

						return true;
					};
				if (!calcAimbot())
				{
					lastLockedAddr = 0;
					initialDistance = 0.f;
				}
			}
		}

		remainX += exactMoveX;
		remainY += exactMoveY;

		auto now_time = std::chrono::steady_clock::now();
		long long elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(now_time - last_mouse_time).count();

		if (elapsed_us >= next_mouse_interval)
		{
			int moveX = static_cast<int>(remainX);
			int moveY = static_cast<int>(remainY);

			remainX -= moveX;
			remainY -= moveY;

			if (moveX != 0 || moveY != 0) {
				MoveMouse(moveX, moveY);
				// printf("[DEBUG] Aimbot moving");
			}

			last_mouse_time = now_time;
			if (GetRandomInt(1, 100) <= 90) {
				next_mouse_interval = GetRandomInt(1, 1000);
			}
			else {
				next_mouse_interval = GetRandomInt(990, 1047);
			}
		}
	}
}
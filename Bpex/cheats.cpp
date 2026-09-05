#define IMGUI_DEFINE_MATH_OPERATORS
#include "cheats.hpp"

std::mutex Cheat::taskMutex;
std::condition_variable Cheat::taskCV;
std::atomic<bool> Cheat::running{ true };

namespace GlobalBuffer {
	std::vector<Player> poolA;
	std::vector<Player> poolB;

	std::atomic<std::vector<Player>*> frontBuffer{ &poolA };

	std::atomic<std::vector<Player>*> backBuffer{ &poolB };

	Player localPlayerSnapshot;
}

// 矩阵线程
void Cheat::MatrixUpdater() {
	uint64_t viewRender = drv.RPM<uint64_t>(Global::GameBase + VIEW_RENDER);
	uint64_t viewMatrixPtr = drv.RPM<uint64_t>(viewRender + VIEW_MATRIX);

	while (running) {

		Global::ViewMatrix = drv.RPM<Matrix>(viewMatrixPtr);
		Global::MatrixReady = true;

		std::this_thread::yield();
	}
}

// 玩家数据线程
void Cheat::WorkerThread() {
	while (running) {
		uintptr_t localPtr = GetLocalPlayerPtr();
		if (!localPtr) {
			GlobalBuffer::frontBuffer.load()->clear();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		Player local;
		local.Ptr = localPtr;
		local.Position = GetPosition(localPtr);
		local.TeamID = GetTeamID(localPtr);
		GlobalBuffer::localPlayerSnapshot = local;

		std::vector<Player>* workingList = GlobalBuffer::backBuffer.load();
		workingList->clear();

		std::string levelName = GetLevelName();
		int loopSize = (levelName == "mp_rr_canyonlands_staging_mu1") ? 10000 : 64;

		for (int i = 0; i < loopSize; i++) {
			uintptr_t ent = GetEntityPtr(i);
			if (!ent || ent == localPtr) continue;

			std::string signifier = GetSignifier(ent);
			if (signifier != "npc_dummie" && signifier != "player") continue;

			Vector3 entPos = GetPosition(ent);
			float dist = GameUnitsToMeters(GetDistance(local.Position, entPos));
			if (dist > cfg::RenderDistance) continue;

			if (GetTeamID(ent) == local.TeamID) continue;

			if (GetLifeState(ent) != 0) continue;

			Player p;
			p.Ptr = ent;
			p.Position = entPos;
			p.ToDistance = dist;
			p.IsVisible = GetVisible(localPtr, ent);
			p.Health = GetHealth(ent);
			p.Shield = drv.RPM<int>(ent + iShield);
			p.MaxShield = drv.RPM<int>(ent + iMaxShield);
			p.IsKnocked = GetKnocked(ent);
			p.TeamID = GetTeamID(ent);

			workingList->push_back(p);
		}

		GlobalBuffer::backBuffer.store(GlobalBuffer::frontBuffer.exchange(workingList));

		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}

// 判断绘制的方框是否在视野内
bool Cheat::IsBoxValid(ImVec4 box)
{
	if (box.x == 0 && box.z == 0) return false;
	if (box.y == 0 && box.w == 0) return false;
	return true;
}

void Cheat::Run()
{
	std::vector<Player>* renderList = GlobalBuffer::frontBuffer.load();
	Player localPlayer = GlobalBuffer::localPlayerSnapshot;

	for (const auto& plyer : *renderList)
	{
		if (cfg::radarEnabled)
		{
			// 这里是400x400像素大小的窗口，如果你找到游戏小地
			// 图比例和小地图的实际大小，可以把这个窗口调整并且
			// 覆盖到小地图雷达上，这是一个简单的外部小雷达实现
			ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(0.1f);
			ImGui::Begin("Radar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			{
				ImVec2 WindowPos = ImGui::GetWindowPos();
				ImVec2 WindowSize = ImGui::GetWindowSize();

				// 雷达十字
				ImGui::GetWindowDrawList()->AddLine(
					WindowPos + ImVec2(WindowSize.x * 0.5f, 0),
					WindowPos + ImVec2(WindowSize.x * 0.5f, WindowSize.y),
					ImColor(255, 255, 255, 75), 1.f);
				ImGui::GetWindowDrawList()->AddLine(
					WindowPos + ImVec2(0, WindowSize.y * 0.5f),
					WindowPos + ImVec2(WindowSize.x, WindowSize.y * 0.5f),
					ImColor(255, 255, 255, 75), 1.f);
				// 雷达玩家点
				Vector2 radarPos = CalcRadarPos(plyer.Position, localPlayer.Position, GetViewAngle(localPlayer.Ptr).y, 30.f);
				if (plyer.TeamID != localPlayer.TeamID)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(WindowPos + ImVec2(200 + radarPos.x, 200 + radarPos.y), 6.f, ImColor(0, 0, 0, 255));
					ImGui::GetWindowDrawList()->AddCircleFilled(WindowPos + ImVec2(200 + radarPos.x, 200 + radarPos.y), 5.f, ImColor(255, 0, 0, 255));
				}
			}
			ImGui::End();
		}

		ImVec4 box = CalcRect(plyer, Global::ViewMatrix);

		if (!IsBoxValid(box)) continue;

		if (cfg::BoxESP)
		{
			// 绘制方框，如果玩家被击倒并且启用了击倒ESP，则使用击倒颜色，否则根据玩家是否可见选择颜色
			ImColor color = (plyer.IsKnocked && cfg::KnockESP) ? cfg::KnockColor : plyer.IsVisible ? cfg::BoxColor : cfg::UnvisColor;
			Draw::DrawBox(box.x, box.y, box.z, box.w, color, 1.f, false);
		}

		if (cfg::LineESP)
		{
			// 射线颜色同理
			ImColor color = (plyer.IsKnocked && cfg::KnockESP) ? cfg::KnockColor : plyer.IsVisible ? cfg::LineColor : cfg::UnvisColor;
			Draw::DrawLine(ImVec2((box.x + box.z) / 2.f, box.w),
				ImVec2(Global::SightCenter.x, Global::ScreenSize.y),color,
				1.f, false);
		}

		// 血量、护甲、队伍标签
		if (cfg::HealthESP)
			Draw::DrawHealth(ImVec2(box.x, box.y), ImVec2(box.z, box.w), plyer.Health);
		if (cfg::ArmorESP)
			Draw::DrawArmor({ box.x, box.y }, { box.z, box.w }, plyer.Shield, plyer.MaxShield);
		if (cfg::TeamESP)
			Draw::DrawTeam({ box.x, box.y }, { box.z, box.w }, plyer.TeamID, GetTeamColor(plyer.TeamID));		
	}
	DrawMenu();
}

// 3D坐标转换为雷达坐标
Vector2 Cheat::CalcRadarPos(Vector3 playerPos, Vector3 localPos, float localYaw, float radarScale)
{
	Vector2 TmpPos = Vector2(playerPos.x - localPos.x, playerPos.y - localPos.y);
	float absDistance = playerPos.distance_to(localPos);

	float angle = atan2(TmpPos.y, TmpPos.x) * (180.f / 3.1415927f);
	float delta = angle - localYaw + 90.f;

	Vector2 absPos = Vector2(
		absDistance * cos(delta * (3.1415927f / 180.f)),
		absDistance * sin(delta * (3.1415927f / 180.f))
	);
	
	absPos.x /= radarScale;
	absPos.y /= radarScale;
	
	return { absPos.x, -absPos.y };
}

// 3D坐标转换为屏幕方框数据
ImVec4 Cheat::CalcRect(Player entity, Matrix m)
{
	Vector3 bs, hs;
	Vector3 HeadPosition = NewHitbox(entity.Ptr, 0);
	HeadPosition.z += 12.f;
	world_to_screen(entity.Position, m, Global::ScreenSize.x, Global::ScreenSize.y, bs);
	world_to_screen(HeadPosition, m, Global::ScreenSize.x, Global::ScreenSize.y, hs);

	float height = fabsf(fabsf(hs.y) - fabsf(bs.y));
	float width = height / 2.f;
	float boxLeft = bs.x - (width / 2);
	float boxRight = bs.x + (width / 2);
	int widthC = (int)boxRight - (int)boxLeft;
	int x1, y1, x2, y2;
	x1 = (int)boxLeft; y1 = (int)hs.y; x2 = (int)boxRight; y2 = (int)bs.y;

	return ImVec4((int)boxLeft, (int)hs.y, (int)boxRight, (int)bs.y);
}

Vector3 Cheat::GetViewAngle(uint64_t LocalPlayerPtr)
{
	return drv.RPM<Vector3>(LocalPlayerPtr + ViewAngle);
}

// 伪随机颜色生成函数，确保同一队伍的颜色一致
ImColor Cheat::GetTeamColor(int TeamID)
{
	std::mt19937 rng(TeamID);
	std::uniform_int_distribution<int> dist(30, 200);

	int r, g, b;
	while (true) {
		r = dist(rng);
		g = dist(rng);
		b = dist(rng);

		// 防止颜色过亮
		float luminance = 0.299f * r + 0.587f * g + 0.114f * b;
		if (luminance < 180.0f) {
			break;
		}
	}

	return ImColor(r, g, b);
}
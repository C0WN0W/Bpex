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


void Cheat::MatrixUpdater() {
	uint64_t viewRender = drv.RPM<uint64_t>(Global::GameBase + VIEW_RENDER);
	uint64_t viewMatrixPtr = drv.RPM<uint64_t>(viewRender + VIEW_MATRIX);

	while (running) {

		Global::ViewMatrix = drv.RPM<Matrix>(viewMatrixPtr);
		Global::MatrixReady = true;

		std::this_thread::yield();
	}
}

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
		//local.Zoomed = drv.RPM<int>(localPtr + Offset::Player::Zooming);
		//local.CameraPos = drv.RPM<Vector3>(localPtr + Offset::Player::CameraPos);
		GlobalBuffer::localPlayerSnapshot = local;

		std::vector<Player>* workingList = GlobalBuffer::backBuffer.load();
		workingList->clear();

		std::string levelName = GetLevelName();
		// std::cout << levelName << std::endl;
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
			// p.Velocity = drv.RPM<Vector3>(ent + Offset::Player::Velocity);

			workingList->push_back(p);
		}

		GlobalBuffer::backBuffer.store(GlobalBuffer::frontBuffer.exchange(workingList));

		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}

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
		ImVec4 box = CalcRect(plyer, Global::ViewMatrix);

		if (!IsBoxValid(box)) continue;

		if (cfg::BoxESP)
		{
			ImColor color = (plyer.IsKnocked && cfg::KnockESP) ? cfg::KnockColor : plyer.IsVisible ? cfg::BoxColor : cfg::UnvisColor;
			Draw::DrawBox(box.x, box.y, box.z, box.w, color, 1.f, false);
		}

		if (cfg::LineESP)
		{
			ImColor color = (plyer.IsKnocked && cfg::KnockESP) ? cfg::KnockColor : plyer.IsVisible ? cfg::LineColor : cfg::UnvisColor;
			Draw::DrawLine(ImVec2((box.x + box.z) / 2.f, box.w),
				ImVec2(Global::SightCenter.x, Global::ScreenSize.y),color,
				1.f, false);
		}

		if (cfg::HealthESP)
			Draw::DrawHealth(ImVec2(box.x, box.y), ImVec2(box.z, box.w), plyer.Health);
		if (cfg::ArmorESP)
			Draw::DrawArmor({ box.x, box.y }, { box.z, box.w }, plyer.Shield, plyer.MaxShield);
	}

	DrawMenu();
}

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
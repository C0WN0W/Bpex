#pragma once
#include "OS-ImGui/OS-ImGui.h"
#include "structs.hpp"

namespace Global
{
	inline uintptr_t GameBase = 0;
	inline Vector2 ScreenSize = { 0,0 };
	inline Vector2 SightCenter = { 0,0 };
	inline Matrix ViewMatrix;
	inline std::atomic<bool> MatrixReady{ false };
}

namespace Game
{
	inline uint64_t localPlayerPtr = 0;
}

namespace cfg
{
	inline bool BoxESP = true;
	inline bool LineESP = false;
	inline bool HealthESP = false;
	inline bool ArmorESP = false;
	inline bool KnockESP = true;
	inline bool TeamESP = false;
	inline int RenderDistance = 300;
	inline bool VisCheck = true;

	inline ImColor BoxColor = ImColor(0, 255, 0, 255);
	inline ImColor LineColor = ImColor(0, 255, 0, 255);
	inline ImColor KnockColor = ImColor(255, 255, 0, 255);
	inline ImColor UnvisColor = ImColor(255, 255, 255, 255);

	inline bool rcs = false;

	inline bool aimbotEnabled = false;
	inline bool NotAimAtKnocked = false;
	inline bool drawFov = true;
	inline float aimbotFov = 100.f;
	inline float aimbotFov_ADS = 60.f;
	inline float aimbotSmooth = 5.f;
	inline float aimbotSmooth_zoomed = 15.f;

	inline bool Triggerbot = false;
	inline int TriggerbotKey = VK_XBUTTON2;
	inline float TriggerbotDist = 10.f;

	inline bool radarEnabled = false;
}
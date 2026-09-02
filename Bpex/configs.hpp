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

	inline int GlowID = 0;
	inline int current_fps = 0;

	inline bool MenuOpen = true;

	inline bool deviceConnected = false;
	inline int device_select = 0;
	inline char Km_IP[50] = { "192.168.2.188" };
	inline char Km_port[50] = { "50204" };
	inline char Km_mac[50] = { "E49E3CAB" };
}

namespace Game
{
	inline uint64_t localPlayerPtr = 0;
	inline uint64_t pInventory = 0;
	inline uint64_t pWeapon = 0;
	inline uint64_t WeaponHandle = 0;
	inline uint64_t WeaponEntity = 0;
}

namespace cfg
{
	inline bool BoxESP = true;
	inline bool LineESP = false;
	inline bool HealthESP = false;
	inline bool ArmorESP = false;
	inline bool KnockESP = true;
	inline bool TeamESP = false;
	inline bool BoneESP = false;
	inline int RenderDistance = 300;
	inline bool VisCheck = true;

	inline ImColor BoxColor = ImColor(0, 255, 0, 255);
	inline ImColor LineColor = ImColor(0, 255, 0, 255);
	inline ImColor KnockColor = ImColor(255, 255, 0, 255);
	inline ImColor BoneColor = ImColor(0, 255, 0, 255);
	inline ImColor UnvisColor = ImColor(255, 255, 255, 255);

	inline bool rcs = false;

	inline bool aimbotEnabled = true;
	inline bool NotAimAtKnocked = false;
	inline bool drawFov = true;
	inline float aimbotFov = 100.f;
	inline float aimbotFov_ADS = 60.f;
	inline float aimbotSmooth = 25.f;
	inline float aimbotSmooth_zoomed = 15.f;

	inline bool radarEnabled = false;
}
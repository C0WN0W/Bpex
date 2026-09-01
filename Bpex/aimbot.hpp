#pragma once
#include <chrono>
#include <ctime>
#include <vector>
#include <algorithm>
#include "entity.hpp"

namespace GlobalBuffer {
	extern std::vector<Player> poolA;
	extern std::vector<Player> poolB;

	extern std::atomic<std::vector<Player>*> frontBuffer;

	extern std::atomic<std::vector<Player>*> backBuffer;

	extern Player localPlayerSnapshot;
}

namespace Aimbot
{
	struct TargetBoneInfo {
		int hitboxId;
		float radiusUnit;
		float customRatio;
		int offsetCount;
	};

	inline float Lerp(float t, float a, float b) {
		return a + t * (b - a);
	}

	int GetRandomInt(int minVal, int maxVal);

	bool KmNetInit(char* ip, char* port, char* mac);

	// Aimbot based on matrix
	float CalcAimHeight(RectData rect, float AimPos, bool isKnocked);
	Vector2 CalcAimPos(RectData rect, bool isKnocked, int duckState);
	Vector2 BestTarget(const std::vector<Vector2>& aimPoints);
	void MouseAimbot(const std::vector<Vector2>& aimPoints, float smooth);

	Vector3 BoneToScreen(uintptr_t entityPtr, Matrix m, int BoneID);

	void MoveMouse(float x, float y);

	void Run();
}
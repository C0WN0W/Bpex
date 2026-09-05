#pragma once
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <atomic>
#include <queue>
#include <cmath>
#include <condition_variable>
#include <random>

#include "renders.hpp"
#include "menu.hpp"
#include "entity.hpp"

struct UpdateTask {
	int loopsize;
	Player localPlayer;
	Matrix m;
};

namespace Cheat
{
	extern std::queue<UpdateTask> taskQueue;
	extern std::mutex taskMutex;
	extern std::condition_variable taskCV;


	extern std::vector<Player> bufferA;
	extern std::vector<Player> bufferB;
	extern std::atomic<bool> useA;
	extern std::atomic<bool> running;

	inline bool SetColored = false;

	void Run();
	void WorkerThread();
	void AimbotThread();
	void MatrixUpdater();

	ImVec4 CalcRect(Player entity, Matrix m);
	bool IsBoxValid(ImVec4 box);
	Vector2 CalcRadarPos(Vector3 entityPos, Vector3 localPos, float localYaw, float radarScale);

	Vector3 GetViewAngle(uint64_t LocalPlayerPtr);
	ImColor GetTeamColor(int TeamID);
}

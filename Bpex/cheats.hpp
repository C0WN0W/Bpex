#pragma once
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

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
	bool ClacRect(RectData rect, ScreenPosData& screen, bool isKnocked);
	bool IsBoxValid(ImVec4 box);
}

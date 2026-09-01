#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>

#include "cheats.hpp"
#include "aimbot.hpp"

using namespace std;
HWND hwnd = 0;

int main()
{
	SetConsoleTitleA("QQ");

	Global::ScreenSize = { static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) };
	Global::SightCenter = { Global::ScreenSize.x / 2.f,Global::ScreenSize.y / 2.f };

	if (!drv.IsInstall()) {
		cout << "驱动未安装..." << endl;
		system("pause > nul");
		return 1;
	}

	int pid = getprocessid(L"r5apex_dx12.exe");
	while (!pid) {
		cout << "等待游戏启动..." << endl;
		Sleep(3000);
		pid = getprocessid(L"r5apex_dx12.exe");
	}
	drv.proceint(pid);

	Global::GameBase = drv.GetMoudleBase();
	if (Global::GameBase == 0) {
		cout << "获取模块地址失败." << endl;
		system("pause > nul");
		exit(0);
	}

	system("cls");

	cout << "模块地址: " << hex << Global::GameBase << endl;

	while (!hwnd)
	{
		hwnd = FindWindowA("Respawn001", "Apex Legends");
		Sleep(2000);
	}
	cout << "HWND: " << hwnd << endl;

	drv.HideProcessAdd(GetCurrentProcessId(), true);

	try {
		std::thread matrixTh(Cheat::MatrixUpdater);
		SetThreadPriority(matrixTh.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
		matrixTh.detach();

		std::thread aimbot(Aimbot::Run);
		aimbot.detach();

		std::thread worker(Cheat::WorkerThread);
		worker.detach();

		// Gui.Attach("Apex Legends", "Respawn001", Cheat::Run);
		Gui.AttachByLyric("Apex Legends", "Respawn001", Cheat::Run);
	}
	catch (OSImGui::OSException& e)
	{
		std::cout << e.what() << std::endl;
	}

	Cheat::running = false;
	Cheat::taskCV.notify_all();

	return 0;
}
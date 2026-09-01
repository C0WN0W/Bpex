#pragma once
#include <iostream>
#include <windows.h>
#include <stdexcept>

typedef void (*SetProc)();
typedef bool (*InstallPtr)();

class DllManager {
public:
    // 获取单例实例
    static DllManager& Instance() {
        static DllManager instance;
        return instance;
    }

    // 全局导出的函数指针（只读访问/通过成员函数调用）
    SetProc SetProcess = nullptr;
    InstallPtr InstallDriver = nullptr;

    // 检查是否加载成功
    bool IsLoaded() const { return hDll != nullptr; }

private:
    HMODULE hDll = nullptr;

    // 构造函数：初始化时只加载一次 DLL 并获取所有函数地址
    DllManager() {
        hDll = LoadLibrary(TEXT("SD.dll"));
        if (!hDll) {
            std::cerr << "加载 SD.dll 失败，错误码: " << GetLastError() << std::endl;
            return;
        }

        // 统一提取地址存入全局可用的成员变量中
        SetProcess = (SetProc)GetProcAddress(hDll, "SetDriverProcess");
        InstallDriver = (InstallPtr)GetProcAddress(hDll, "InstallDriver");

        if (!SetProcess) {
            std::cerr << "警告: SetProcess 函数未找到！" << std::endl;
        }
        if (!InstallDriver) {
            std::cerr << "警告: InstallDriver 函数未找到！" << std::endl;
        }
    }

    ~DllManager() {
        if (hDll) {
            FreeLibrary(hDll);
            hDll = nullptr;
        }
    }

    DllManager(const DllManager&) = delete;
    DllManager& operator=(const DllManager&) = delete;
};


namespace Driver {
	inline void SetDriverProcess() {
		auto pSetProcess = DllManager::Instance().SetProcess;
		if (pSetProcess) {
			pSetProcess();
		}
	}

	inline bool InstallDriver() {
        auto pInstallDriver = DllManager::Instance().InstallDriver;
        if (!pInstallDriver) {
            return false;
        }
        return pInstallDriver();
	}
}
#pragma once
#include <windows.h>
#include <winternl.h>

//免责声明
//本驱动驱动出于个人兴趣爱好作为学习计算机交流使用开发目的，请勿用于非法行为，违规违法项目。
//使用者一切非法行为活动与作者无关。模块使用者得行为不代表作者得立场，模块使用者务必遵守国家
//相关法律法规合理使用本驱动,请勿使用本驱动进行违法行为，否则一律与开发人员和代理人员无关且后果自负。

#if _WIN64
#pragma comment(lib,"2022_MT_x64.lib")

#define PTRMAXVAL ((PVOID64)0x000F000000000000)
#define pVOID PVOID64

#else
#if _WIN32

//#pragma comment(lib,"2019_MD_TX__x86.lib")  没封装 有需要可以封装

#define PTRMAXVAL ((PVOID)0xFFF00000)
#define pVOID PVOID
#endif
#endif
#define IsAddressValid(ptr) (((ptr >= 0x40000) && ((pVOID)ptr < PTRMAXVAL) && ((pVOID)ptr != nullptr)) ? TRUE : FALSE)

#pragma pack(push, 2)

typedef LARGE_INTEGER PHYSICAL_ADDRESS, * PPHYSICAL_ADDRESS;

typedef struct _PHYSICAL_MEMORY_RANGE {
	PHYSICAL_ADDRESS BaseAddress;
	LARGE_INTEGER NumberOfBytes;
} PHYSICAL_MEMORY_RANGE, * PPHYSICAL_MEMORY_RANGE;

typedef struct _SYSTEM_MEMORY_INFO
{
	LARGE_INTEGER CR3;
	LARGE_INTEGER NtBuildNumber;
	LARGE_INTEGER KernBase;
	LARGE_INTEGER KPCR[64];
	LARGE_INTEGER NtBuildNumberAddr;
	LARGE_INTEGER NumberOfRuns;
	PHYSICAL_MEMORY_RANGE Run[20];
} SYSTEM_MEMORY_INFO, * PSYSTEM_MEMORY_INFO;
#pragma pack(pop)
 
typedef unsigned long long u64;
typedef unsigned long u32;

typedef PVOID64 ptr;
typedef PVOID64 p;
typedef u32* dptr;
typedef u64* qptr;

class Driver
{
public:
	Driver();
	~Driver();
public:
    //安装驱动
	BOOL Loaddriver(const char* key);
	//卸载驱动
	void UnDriver();
	//取进程PEB
	u64 GetPEB(u32 pid);	
	//驱动是否安装
	BOOL IsInstall();
	//进程保护隐藏开启 参数1 进程标识  参数2 是否隐藏进程  （支持多个进程同时操作）
	void HideProcessAdd(u32 pid, BOOL ishi);
	//进程保护隐藏恢复 参数1 进程标识  参数2 是否隐藏进程  （支持多个进程同时操作）
	void HideProcessSub(u32 pid, BOOL ishi);
	//鼠标移动
	void Mouse_move(long x, long y);
	//鼠标移动2
	void Mouse_move2(long x, long y);
	//读内核内存
	void ReadKernelMemory(u64 addr, ptr buffer, u64 size);
	//读取R3内存  模式 0默认 1 MDL  2 物理  3 无附加物理
	BOOL ReadProcessMemory(u64 addr, ptr buffer, u64 size, u32 moshi);
	//写入R3内存  模式 0默认 1 MDL  2 物理  3 无附加物理
	BOOL WriteProcessMemory(u64 addr, ptr buffer, u64 nSize, u32 moshi);	
	//通过PTE修改可执行 隐藏内存可执行属性 并保持可执行状态
	void HideMemory(u32 PID, DWORD64 Memory);
	//申请内存
	u64 AllocateVirtualMemory(u32 pid, ptr addr, u32 ZeroBits, u32 Size, u32 AllocationType, u32 Protect);
	//申请MDL映射内存
	u64 AllocateVirtualMemoryA(u32 pid, u32 Size, u32 Protect);
	//释放内存
	void FreeVirtualMemory(u32 pid, ptr addr, u64 Size, u32 FreeType);
	//远程创建线程方式1 隐蔽方法1 user32模块跳转 + ZwCreateThreadEx
	void CreateThreadEx(u32 pid, ptr start);
	//远程创建线程方式2 隐蔽方法2  未公开函数
	void CreateThread(u32 pid, ptr start, ptr parameter);
	//取模块函数地址
	u64 GetModuleExportAddress(u32 pid, ptr base, CHAR* name);
	//设置进程
	BOOL proceint(u32 pid);
	//取主模块地址
	u64 GetMoudleBase();
	//取模块地址
	u64 GetMoudleEx(CHAR* Ming);
	//取模块大小
	u64 GetMoudleSize(CHAR* Ming);
	//x64注入 参数1 进程pid 参数2 文件指针 参数3 文件大小 参数4 类型 （ 0=消息 1=劫持Rip 2=APC插入 3=APC隐藏 ） 参数5  是否使用映射内存
	u64 Tracelessinjection(u32 pid, ptr moudle, u32 Size, u32 InjectType, BOOL ismdl);
	//x64线程注入 文件绝对路径
	BOOL InjectDll(u32 pid, CHAR* name);
	//强删文件
	void DeleteFileEx(CHAR* name);
	//锁定文件  支持多个文件
	BOOL LockFileEx(CHAR* name);
	//解锁全部文件
	void ULockFileAll();
	//修改内存属性
	BOOL ProtectVirtualMemory(u32 pid, u64 addr, u32 ProtectSize, u32 NewProtect, u32* OldProtect);
	//查询内存 参数1=进程pid 参数2=查询地址   参数3  PMEMORY_BASIC_INFORMATION
	void QueryVirtualMemory(u32 pid, ptr addr, ptr info);
	//远程汇编 绑定主线程
	void RemoteCall(u32 pid, u32 tid, u64 shellcode, u64 len);
	//特征码搜索
	u64 FindProcSignCode(u32 pid, u64 base, CHAR* code, u64 len);
	//防止截图 参数1 窗口句柄 参数2 用17
	u32 Protect_sprite_content(u64 handle, u32 attributes);
	
	//进程伪装 参数1 欲伪装的进程 参数2 目标进程
	BOOL FakeProcess(u32 pid, u32 SrcPid);
	//线程伪装
	void HideThread(u32 tid);
	
	//恢复挂起进程
	void ResumeProcess(u32 pid);
	//挂起进程
	void SuspendProcess(u32 pid);
	
	//挂起线程
	void SuspendThread(u32 ThreadId);
	//恢复挂起线程
	void ResumeThread(u32 ThreadId);
	
	//键盘按下
	void KeyDown(USHORT VirtualKey);
	//键盘弹起
	void KeyUp(USHORT VirtualKey);
	//鼠标侧键1（XButton1）按下
	void MouseXButton1Down();
	//鼠标侧键1（XButton1）释放
	void MouseXButton1Up();
	//鼠标侧键2（XButton2）按下
	void MouseXButton2Down();
	//鼠标侧键2（XButton2）释放
	void MouseXButton2Up();
	//鼠标左键按下
	void MouseLeftButtonDown();
	//鼠标左键弹起
	void MouseLeftButtonUp();
	//鼠标右键按下
	void MouseRightButtonDown();
	//鼠标右键弹起
	void MouseRightButtonUp();
	//鼠标中键按下
	void MouseMiddleButtonDown();
	//鼠标中键弹起
	void MouseMiddleButtonUp();
	//鼠标相对移动
	void MouseMoveRELATIVE(LONG dx, LONG dy);
	//鼠标绝对
	void MouseMoveABSOLUTE(LONG dx, LONG dy);
	//向上滚动垂直滚轮
	void ScrollVerticalUp(USHORT units);
	//向上滚动垂直滚轮
	void ScrollVerticalDown(USHORT units);	
	//取系统相关信息
	void GetSystemInfo(PSYSTEM_MEMORY_INFO data);
	//JD初始解密
	void InitializeDecrypt(u64 base);
	//JD通用解密
	u64 KernelDecrypt(u64 buff);
	
	
	void SetContextThread(HANDLE ThreadId, PCONTEXT Context);
	
	void GetContextThread(HANDLE ThreadId, PCONTEXT Context);
	
	
	BOOL ValidPtr(ULONG64 Ptr, ULONG a = 0)
	{

		return (BOOL)!IsAddressValid(Ptr);// (BOOL)(Ptr < 0xFFFF || Ptr > 0x7FFFFFFFFFFF || Ptr % a);
	}

	//绘制矩形
	//void DrawBox(u32 x, u32 y, u32 w, u32 h, u32 thickness, u32 r, u32 g, u32 b);
	template<typename T>
	T RPM(unsigned long long Addr);

	template<typename T>
	T RPM(unsigned long long Addr, unsigned long Size);

	template<typename T>
	bool RPM(unsigned long long Addr, T OuterBuffer, unsigned long Size);

	template<typename T>
	bool WPM(unsigned long long Addr, T value);

	template<typename T>
	bool WPM(unsigned long long Addr, T value, unsigned long Size);

};


	// std::string ReadName(ULONG value)
	// {
		// struct FText
		// {
			// char buffer[64];
		// };
		// FText name = drv.RPM<FText>(value);
		// auto rel = std::string(name.buffer);
		// auto pos = rel.rfind('/');
		// if (pos != std::string::npos)
		// {
			// rel = rel.substr(pos + 1);
		// }
	 
		// return std::string(rel);
	// }


template<typename T> inline T Driver::RPM(unsigned long long Addr)
{
	T readBuffer{};
	u64 lpNumberOfBytesRead;
	ReadProcessMemory(Addr, &readBuffer, sizeof(T),0);
	return (T)readBuffer;
}

template<typename T> inline T Driver::RPM(unsigned long long Addr, unsigned long Size)
{
	T readBuffer = {};
	u64 lpNumberOfBytesRead;
	ReadProcessMemory(Addr, &readBuffer, Size,0);
	return (T)readBuffer;
}

template<typename T> inline bool Driver::RPM(unsigned long long Addr, T OuterBuffer, unsigned long Size)
{
	u64 lpNumberOfBytesRead;
	ReadProcessMemory( Addr, OuterBuffer, Size,0);
	return TRUE;
}

template<typename T> inline bool Driver::WPM(unsigned long long Addr, T value)
{
	u64 lpNumberOfBytesRead;
	WriteProcessMemory(Addr, &value, sizeof(T),0);
	return TRUE;
}

template<typename T> inline bool Driver::WPM(unsigned long long Addr, T value, unsigned long Size)
{
	u64 lpNumberOfBytesRead;
	WriteProcessMemory(Addr, &value, Size, 0);
	return TRUE;
}

//不能更改名字 否则无法编译 或者你可以自己 new 一个
extern Driver drv;


#include <TlHelp32.h>
inline int getprocessid(const wchar_t* process_name) {
	PROCESSENTRY32 processentry;
	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot_handle == INVALID_HANDLE_VALUE)
		return 0;

	processentry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot_handle, &processentry)) {
		do {
			if (wcscmp(process_name, processentry.szExeFile) == 0) {
				CloseHandle(snapshot_handle);
				return processentry.th32ProcessID;
			}
		} while (Process32Next(snapshot_handle, &processentry));
	}

	CloseHandle(snapshot_handle);
	return 0;
}

// dllmain.cpp : Defines the entry point for the DLL application.
#include <SDKDDKVer.h>
#include <windows.h>
#include <winternl.h>
#include <TlHelp32.h>

#include "MinHook.h"
#include "log.h"
#include "dll.h"

typedef HRESULT(WINAPI* tDirectInput8Create)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
tDirectInput8Create oDirectInput8Create = nullptr;

void Initialize()
{
	char syspath[MAX_PATH];
	GetSystemDirectory(syspath, MAX_PATH);
	strcat_s(syspath, "\\dinput8.dll");
	HMODULE hMod = LoadLibrary(syspath);
	oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");

	LOG(WARN) << "Quest Loader";
	LOG(ERR) << "Game process will not terminate properly until this window is closed";

	MH_Initialize();
	InjectForceNativePC();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

#ifndef _DEBUG
	min_log_level = INFO;
#else
	min_log_level = DEBUG;
#endif // !_DEBUG
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		Initialize();
	}
	return TRUE;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk)
{
	KillCRCThreads();
	StartTerminateLoop();
	return oDirectInput8Create(inst_handle, version, r_iid, out_wrapper, p_unk);
}

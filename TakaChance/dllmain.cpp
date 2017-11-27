// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

/*
#include "../detours/detourplus.h"
#pragma comment(lib, "../Debug/detours.lib")
*/

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//OutputDebugStringA("TakaChance.Dll is being loading.\n");
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

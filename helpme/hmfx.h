#ifndef ___HELPME_FX_H_INCLUDE___
#define ___HELPME_FX_H_INCLUDE___

#if defined(HOST_X86ONX64)
#include "C:\\Program Files (x86)\\WinDBG\\sdk\\inc\\dbgeng.h"
#include "C:\\Program Files (x86)\\WinDBG\\sdk\\inc\\wdbgexts.h"
#include "C:\\Program Files (x86)\\WinDBG\\sdk\\inc\\dbghelp.h"
#pragma comment(lib, "C:\\Program Files (x86)\\WinDBG\\sdk\\lib\\i386\\dbgeng.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\WinDBG\\sdk\\lib\\i386\\dbghelp.lib")
#else
#include "C:\\Program Files\\Microsoft SDKs\\WinDBGTools\\sdk\\inc\\dbgeng.h"
#include "C:\\Program Files\\Microsoft SDKs\\WinDBGTools\\sdk\\inc\\wdbgexts.h"
#include "C:\\Program Files\\Microsoft SDKs\\WinDBGTools\\sdk\\inc\\dbghelp.h"
#ifdef HOST_X64
#pragma comment(lib, "C:\\Program Files\\WinDBG\\sdk\\lib\\amd64\\dbgeng.lib")
#pragma comment(lib, "C:\\Program Files\\WinDBG\\sdk\\lib\\amd64\\dbghelp.lib")
#else
#pragma comment(lib, "C:\\Program Files\\Microsoft SDKs\\WinDBGTools\\sdk\\lib\\i386\\dbgeng.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft SDKs\\WinDBGTools\\sdk\\lib\\i386\\dbghelp.lib")
#endif
#endif

#endif

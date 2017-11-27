// TakaChance.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <tchar.h>
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "../Debug/detours.lib")

#include "../detours/detourplus.h"

#define _HELPME_USAGE_BEYOND_HELPME_DLL

#define DEBUG_ECHOMSG printf
#include "../helpme/helpmecommon.h"
#include "../helpme/helpmerpc.h"

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
	free(ptr);
}

void __fastcall TakaChanceMain(UUID* pGuid)
{
	TCHAR buf[1024];
	RPC_TSTR str;
	UuidToString(pGuid, &str);
	_stprintf(buf, _T("TakaChanceMain %s\n"), str);
	OutputDebugString(buf);
	_stprintf(buf, HELPME_RPC_ENDPOINT_FORMATSRING, str);
	RPC_STATUS status = RpcServerUseProtseqEp((RPC_TSTR)_T("ncalrpc"), 0, (RPC_TSTR)buf, NULL);
	RpcStringFree(&str);
	_stprintf(buf, _T("RpcServerUseProtseqEp returns %X(%u)\n"), status, status);
	OutputDebugString(buf);
	if(status == 0) {
		status = RpcServerRegisterIf(IHelpmeMain_v1111_2222_s_ifspec, NULL, NULL);
		_stprintf(buf, _T("RpcServerRegisterIf for Main returns %X(%u)\n"), status, status);
		OutputDebugString(buf);
		if(status == 0) {
			status = RpcServerRegisterIf(IHelpmeDetour_v1111_2222_s_ifspec, NULL, NULL);
			_stprintf(buf, _T("RpcServerRegisterIf for Detour returns %X(%u)\n"), status, status);
			OutputDebugString(buf);
			if(status == 0) {
				status = RpcServerListen(1, 1, FALSE);
				_stprintf(buf, _T("RpcServerListen returns %X(%u)\n"), status, status);
				OutputDebugString(buf);
			}
		}
	}
}

SimpleLinked<GUID> *gp_clients;

/* [async] */ void  HlpmConnect( 
	/* [in] */ PRPC_ASYNC_STATE HlpmConnect_AsyncHandle,
	/* [in] */ REFGUID rguidService,
	/* [out] */ GUID *pguidId)
{
	TCHAR buf[384];	
	OutputDebugString(_T("Querying connection for "));
	RPC_TSTR str;
	UuidToString(&rguidService, &str);
	OutputDebugString((LPTSTR)str);
	OutputDebugString(_T(" ...\n"));
	RpcStringFree(&str);
	UuidCreate(pguidId);
	SimpleLinked<GUID> *ptr = new SimpleLinked<GUID>(*pguidId);
	if(!gp_clients)
		gp_clients = ptr;
	else
		gp_clients->Add(ptr);

	OutputDebugString(_T("Created connection: "));
	UuidToString(&ptr->value, &str);
	OutputDebugString((LPTSTR)str);
	RpcStringFree(&str);
	OutputDebugString(_T("\n"));
	RPC_STATUS status = RpcAsyncCompleteCall(HlpmConnect_AsyncHandle, buf);
	_stprintf(buf, _T("RpcAsyncCompleteCall returns %x(%u)\n"), status, status);
	OutputDebugString(buf);
}

/* [async] */ void  HlpmDisconnect( 
	/* [in] */ PRPC_ASYNC_STATE HlpmDisconnect_AsyncHandle,
	/* [in] */ REFGUID rguidId)
{
	OutputDebugString(_T("Querying disconnection ...\n"));
	if(!gp_clients) {
		OutputDebugString(_T("Never connected, nothing to do\n"));
		return;
	}
		
	RPC_TSTR str;
	UuidToString(&rguidId, &str);	
	OutputDebugString(_T("Searching connection: "));
	OutputDebugString((LPTSTR)str);
	OutputDebugString(_T("\n"));
	RpcStringFree(&str);
	SimpleLinked<GUID>* ptr = gp_clients->Remove(rguidId);
	if(ptr) {
		OutputDebugString(_T("Found!, disconnected!\n"));
		delete ptr;		
	}
	else
		OutputDebugString(_T("Connection not existing\n"));
	RPC_STATUS status = RpcAsyncCompleteCall(HlpmDisconnect_AsyncHandle, NULL);
	TCHAR buf[384];
	_stprintf(buf, _T("RpcAsyncCompleteCall returns %x(%u)\n"), status, status);
	OutputDebugString(buf);	
	
	status = RpcMgmtStopServerListening(NULL);
	_stprintf(buf, _T("RpcMgmtStopServerListening returns %x(%u)\n"), status, status);
	OutputDebugString(buf);

	status = RpcServerUnregisterIf(IHelpmeDetour_v1111_2222_s_ifspec, NULL, FALSE);
	_stprintf(buf, _T("RpcServerUnregisterIf for Detour returns %x(%u)\n"), status, status);

	status = RpcServerUnregisterIf(IHelpmeMain_v1111_2222_s_ifspec, NULL, FALSE);
	_stprintf(buf, _T("RpcServerUnregisterIf for Main returns %x(%u)\n"), status, status);
	OutputDebugString(buf);
}

/* [async] */ void  HlpmLoadLibrary( 
	/* [in] */ PRPC_ASYNC_STATE HlpmLoadLibrary_AsyncHandle,
	/* [in] */ BSTR bstrPath,
	/* [out] */ DWORD *pdwErr)
{
	RpcTryExcept
		OutputDebugString(_T("LoadLibrary for "));
		OutputDebugStringW(bstrPath);
		OutputDebugString(_T("\n"));
		SetLastError(0);
		LoadLibraryW(bstrPath);
		*pdwErr = GetLastError();
		RpcAsyncCompleteCall(HlpmLoadLibrary_AsyncHandle, NULL);
	RpcExcept(1)
	RpcEndExcept
}

struct DetourPair
{
	ULONG64 target;
	ULONG64 source;
};

SimpleLinked<DetourPair> *gp_detourpair;

/* [async] */ void  HlpmLoadLibraryEx( 
	/* [in] */ PRPC_ASYNC_STATE HlpmLoadLibraryEx_AsyncHandle,
	/* [in] */ BSTR bstrPath,
	/* [in] */ DWORD dwFlags,
	/* [out] */ DWORD *pdwErr)
{
	RpcTryExcept
		OutputDebugString(_T("LoadLibraryEx for "));
		OutputDebugStringW(bstrPath);
		OutputDebugString(_T("\n"));
		SetLastError(0);
		LoadLibraryExW(bstrPath, NULL, dwFlags);
		*pdwErr = GetLastError();
		RpcAsyncCompleteCall(HlpmLoadLibraryEx_AsyncHandle, NULL);
	RpcExcept(1)
	RpcEndExcept
}

typedef void (__stdcall *PFN_ToBeDetour)(void);
PFN_ToBeDetour pfnToBeDetour;

/* [async] */ void  HlpmDetourAttach( 
	/* [in] */ PRPC_ASYNC_STATE HlpmDetour_AsyncHandle,
	/* [in] */ ULONG64 pfnTarget,
	/* [in] */ ULONG64 pfnSource,
	/* [in] */ DWORD dwThreadId,
	/* [out] */ ULONG64 *Id)
{
	TCHAR buf[1024];
	if((pfnTarget | pfnSource) & 0xFFFFFFFF00000000ull) {
		OutputDebugString(_T("X64 target is not supported yet now!\n"));
		RpcAsyncCompleteCall(HlpmDetour_AsyncHandle, NULL);
		return;
	}
	if(dwThreadId == 0) {
		dwThreadId = GetCurrentThreadId();
		OutputDebugString(_T("Origin tid is 0, reset to current tid!\n"));
	}
	_stprintf(buf, _T("The tid is %u(0x%x)!\n"), dwThreadId, dwThreadId);
	OutputDebugString(buf);
	OutputDebugString(_T("-- tid for detour is ignored!, just use current thread. --\n"));
	SimpleLinked<DetourPair>* psldp = SimpleLinked<DetourPair>::AllocRaw();
	psldp->value.target = pfnTarget;
	psldp->value.source = pfnSource;
	pfnToBeDetour = (PFN_ToBeDetour)pfnTarget;
	RpcTryExcept
		DETOUR_BEGIN(dwThreadId);
		DETOUR_ATTACH(psldp->value.target, psldp->value.source);
		DETOUR_COMMIT();
		SimpleLinked_SetOrAdd(&gp_detourpair, psldp);
		ULONG64* p = (ULONG64*)HlpmDetour_AsyncHandle->UserInfo;
		if(p) { *p = (ULONG64)(INT_PTR)psldp; *Id = (ULONG64)(INT_PTR)psldp;}
		_stprintf(buf, _T("set gp_detourpair to %p\n"), gp_detourpair);
		OutputDebugString(buf);
		RpcAsyncCompleteCall(HlpmDetour_AsyncHandle, NULL);
	RpcExcept(1)
	RpcEndExcept
}

/* [async] */ void  HlpmDetourDetach( 
	/* [in] */ PRPC_ASYNC_STATE HlpmDetourDetach_AsyncHandle,
	/* [in] */ ULONG64 Id)
{
	if(Id != 0) {
		SimpleLinked<DetourPair>* p = (SimpleLinked<DetourPair>*)(INT_PTR)Id;
		p = gp_detourpair->Search(p->value);
		if(p) {
			DETOUR_BEGIN();
			DETOUR_DETACH(p->value.target, p->value.source);
			DETOUR_COMMIT();
			TCHAR buf[512];
			_stprintf(buf, _T("Detour detach for %I64p completed.\n"), p);
			OutputDebugString(buf);
		}
	}
	else
		OutputDebugString(_T("Invalid Id for detour detach.\n"));
	RpcAsyncCompleteCall(HlpmDetourDetach_AsyncHandle, NULL);
}

/* [async] */ void  HlpmTestDetour( 
	/* [in] */ PRPC_ASYNC_STATE HlpmTestDetour_AsyncHandle)
{
	if(pfnToBeDetour)
		pfnToBeDetour();
	RpcAsyncCompleteCall(HlpmTestDetour_AsyncHandle, NULL);
}

void __stdcall ToBeDetour(void)
{
	OutputDebugStringW(L"THIS IS FROM TAKACHANCE.DLL::ToBeDetour\n");
}


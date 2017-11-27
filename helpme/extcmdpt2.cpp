#include "stdafx.h"
#include "helpme.h"
#include "helpmerpc.h"


__forceinline
RPC_STATUS InitAsyncStateWithEvent(
	RPC_ASYNC_STATE &asyncstate, HANDLE hEvent)
{
	RPC_STATUS status = RpcAsyncInitializeHandle(&asyncstate, sizeof(RPC_ASYNC_STATE));
	asyncstate.NotificationType = RPC_NOTIFICATION_TYPES::RpcNotificationTypeEvent;
	asyncstate.u.hEvent = hEvent;
	return status;
}

__forceinline
RPC_STATUS InitAsyncStateWithApc(
	RPC_ASYNC_STATE &asyncstate, HANDLE hThread, PFN_RPCNOTIFICATION_ROUTINE Apc)
{
	RPC_STATUS status = RpcAsyncInitializeHandle(&asyncstate, sizeof(RPC_ASYNC_STATE));
	asyncstate.NotificationType = RPC_NOTIFICATION_TYPES::RpcNotificationTypeApc;
	asyncstate.u.APC.hThread = hThread;
	asyncstate.u.APC.NotificationRoutine = Apc;
	return status;
}

volatile bool _rpchelp_thread_continue = false;

HANDLE _rpchelp_thread_started_event = NULL;
unsigned __stdcall _rpchelp_thread(void* p)
{	
	if(!_rpchelp_thread_started_event)
		return 0xF1EEF1EE;

	SetEvent(_rpchelp_thread_started_event);
	while(_rpchelp_thread_continue) {
		// sleep until an alert arrived
		SleepEx(INFINITE, TRUE);
	}
	ExtensionApis.lpOutputRoutine("Exit rpchelp thread with 0xE217E217\n");
	_endthreadex(0xE217E217);
	return 0xE217E217;
}

static uintptr_t _rpchelp_thread_handle = NULL;
void BeginRpcHelpThread(void)
{	
	if(!_rpchelp_thread_continue) {
		unsigned int addr;
		_rpchelp_thread_continue = true;
		_rpchelp_thread_started_event = CreateEvent(NULL, TRUE, FALSE, NULL);
		_rpchelp_thread_handle  = _beginthreadex(NULL, 0, _rpchelp_thread, NULL, 0, &addr);
		WaitForSingleObject(_rpchelp_thread_started_event, INFINITE);
		ExtensionApis.lpOutputRoutine("Running rpc help thread %x(%u) at %p\n", _rpchelp_thread_handle, _rpchelp_thread_handle, addr);
		CloseHandle(_rpchelp_thread_started_event);
		_rpchelp_thread_started_event = NULL;
	}
}

RPC_STATUS CleanUpRpcResources(void)
{	
	RPC_STATUS status = 0;
	if(HelpmeMain_IfHandle)
		status = RpcBindingFree(&HelpmeMain_IfHandle);
	if(HelpmeDetour_IfHandle)
		status = RpcBindingFree(&HelpmeDetour_IfHandle);
	return status;
}

#define RPC_ASYNC_STATE_COUNT 4
RPC_ASYNC_STATE g_asyncstates[RPC_ASYNC_STATE_COUNT];

UUID RpcEndpointId;
GUID connid;

void RPC_ENTRY atrpc_callback(
	struct _RPC_ASYNC_STATE *pAsync,
	void *Context,
	RPC_ASYNC_EVENT Event)
{
	if(Event == RPC_ASYNC_EVENT::RpcCallComplete)
	{
		ExtensionApis.lpOutputRoutine("Connect completed.\n");
		RpcAsyncCompleteCall(pAsync, NULL);
		RPC_CSTR str;
		UuidToStringA(&connid, &str);
		ExtensionApis.lpOutputRoutine("The connection id is: %s\n", str);
		RpcStringFreeA(&str);
	}
}

HELPME_COMMAND(atrpc)
{	
	IDebugControl *dbgCtrl;
	Client->QueryInterface(__uuidof(IDebugControl), (void**)&dbgCtrl);	
	RPC_TSTR binding_string = NULL;
	TCHAR buf[1024];
	UuidToString(&RpcEndpointId, &binding_string);
	_stprintf(buf, HELPME_RPC_ENDPOINT_FORMATSRING, binding_string);	
	RpcStringFree(&binding_string);
	RPC_STATUS status = RpcStringBindingCompose(NULL, (RPC_TSTR)_T("ncalrpc"), NULL, (RPC_TSTR)buf, NULL, &binding_string);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "RpcStringBindingCompose returns %x(%u)\n", status, status);
	if(status == 0) {
		status = RpcBindingFromStringBinding(binding_string, &HelpmeMain_IfHandle);
		dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "RpcBindingFromStringBinding for Main returns %x(%u)\n", status, status);
		if(status == 0) {
			status = RpcBindingFromStringBinding(binding_string, &HelpmeDetour_IfHandle);
			dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "RpcBindingFromStringBinding for Detour returns %x(%u)\n", status, status);			
			BeginRpcHelpThread();
			status = InitAsyncStateWithApc(g_asyncstates[0], (HANDLE)_rpchelp_thread_handle, atrpc_callback);
			if(status == 0) {
				dbgCtrl->Output(DEBUG_OUTPUT_PROMPT, "Run debuggee to take effort!\n");
				RpcTryExcept
					HlpmConnect(&g_asyncstates[0], IID_IUnknown, &connid);
					if(binding_string)
						RpcStringFree(&binding_string);
				RpcExcept(1)
				RpcEndExcept	
			}
		}
	}
	dbgCtrl->Release();
	return status;
}

void RPC_ENTRY dtrpc_callback(
	struct _RPC_ASYNC_STATE *pAsync,
	void *Context,
	RPC_ASYNC_EVENT Event)
{
	if(Event == RPC_ASYNC_EVENT::RpcCallComplete)
	{
		ExtensionApis.lpOutputRoutine("Disconnect finished\n");
		RpcTryExcept
			RpcAsyncCompleteCall(pAsync, NULL);
		RpcExcept(1)
		RpcEndExcept
		memset(&connid, 0, sizeof(GUID));						
		RPC_STATUS status = CleanUpRpcResources();
		ExtensionApis.lpOutputRoutine("RpcBindingFree returns %x(%u)\n", status, status);
		ExtensionApis.lpOutputRoutine("return from rpc help thread %x(%u).\n", _rpchelp_thread_handle, _rpchelp_thread_handle);
		_rpchelp_thread_continue = false;
		_rpchelp_thread_handle = NULL;
	}
}

HELPME_COMMAND(dtrpc)
{	
	IDebugControl *dbgCtrl;
	HRESULT hr = S_OK;
	Client->QueryInterface(__uuidof(IDebugControl), (void**)&dbgCtrl);
	ULONG uval;
	dbgCtrl->GetExecutionStatus(&uval);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Debuggee status is %x(%u)\n", uval, uval);
	if(_rpchelp_thread_continue) {
		hr = InitAsyncStateWithApc(g_asyncstates[1], (HANDLE)_rpchelp_thread_handle, dtrpc_callback);
		if(hr == 0) {
			dbgCtrl->Output(DEBUG_OUTPUT_PROMPT, "Run debuggee to take effect!\n");
			RpcTryExcept
				HlpmDisconnect(&g_asyncstates[1], connid);
			RpcExcept(1)
			RpcEndExcept
		}
		else {
			dbgCtrl->Output(DEBUG_OUTPUT_WARNING, "Init state failed, code is: %x(%u)\n", hr, hr);
		}
	}
	else {
		dbgCtrl->Output(DEBUG_OUTPUT_WARNING, "Help thread is not running, you may not connected ...\n");
		hr = S_FALSE;
	}
	dbgCtrl->Release();
	return hr;
}

HELPME_COMMAND(strpc)
{
	CComQIPtr<IDebugControl> dbgCtrl = Client;
	RPC_CSTR str;
	UuidToStringA(&RpcEndpointId, &str);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Endpoint Id: %s\n", str);
	RpcStringFreeA(&str);
	UuidToStringA(&connid, &str);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Connection Id: %s\n", str);
	RpcStringFreeA(&str);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Main Handle: %p\n", HelpmeMain_IfHandle);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Detour Handle: %p\n", HelpmeDetour_IfHandle);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Thread: %p, State: %d\n", _rpchelp_thread_handle, _rpchelp_thread_continue);	
	return S_OK;	
}

struct StringDwordPair {
	LPTSTR Key;
	DWORD Val;
} LoadLibraryExFlagsMap[] = {
#define NAME_VALUE_PAIR(key) _T(#key), key
	{ NAME_VALUE_PAIR(DONT_RESOLVE_DLL_REFERENCES) },
	{ NAME_VALUE_PAIR(LOAD_LIBRARY_AS_DATAFILE) },
	{ NAME_VALUE_PAIR(LOAD_WITH_ALTERED_SEARCH_PATH) },
	{ NAME_VALUE_PAIR(LOAD_IGNORE_CODE_AUTHZ_LEVEL) },
	{ NAME_VALUE_PAIR(LOAD_LIBRARY_AS_IMAGE_RESOURCE) },
	{ NAME_VALUE_PAIR(LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) },
	{ NAME_VALUE_PAIR(LOAD_LIBRARY_REQUIRE_SIGNED_TARGET) },
#undef NAME_VALUE_PAIR
};

struct ldlib_data {
	BSTR path;
	DWORD err;

	~ldlib_data() {
		if(path)
			SysFreeString(path);
		path = NULL;
	}
};

void RPC_ENTRY ldlib_callback(
struct _RPC_ASYNC_STATE *pAsync,
	void *Context,
	RPC_ASYNC_EVENT Event)
{
	if(Event == RPC_ASYNC_EVENT::RpcCallComplete)
	{
		ExtensionApis.lpOutputRoutine("LoadLibrary completed, and\n");
		RpcAsyncCompleteCall(pAsync, NULL);
		ldlib_data* pd = (ldlib_data*)(pAsync->UserInfo);
		ExtensionApis.lpOutputRoutine("returns error code: %x(%u)\n", pd->err, pd->err);
		delete pd;
		pAsync->UserInfo = NULL;
	}
}
/*
 * Description:
 *     Load specified library (dll) into debugee's address space.
 *     It works like LoadLibrary Win32 API.
 * Usage:
 *     !ldlib <LibraryPath> [flags]
 *     NOTE: arguments Must in this order.
 *     flags - the LoadLibraryEx's dwFlags, can be digital value or literal
 *	         - if the digit is begin with 'a','b','c','d','e','f', "0x" prefix must use
 *     LibraryPath - the library's path
 *     
 * Example:
 *     !ldlib A:\helpme.dll
 *     !ldlib A:\helpme.dll 1
 *     !ldlib A:\helpme.dll 0xC
 *     !ldlib A:\helpme.dll DONT_RESOLVE_DLL_REFERENCE 
 *     !ldlib A:\helpme.dll DONT_RESOLVE_DLL_REFERENCE|0x1 
 */
HELPME_COMMAND(ldlib)
{
	IDebugControl4* dbgCtrl4;
	Client->QueryInterface(__uuidof(IDebugControl4), (void**)&dbgCtrl4);
	if(!_rpchelp_thread_continue) {
		dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "Thread has been terminated, ldlib will do nothing!\n");
		return S_FALSE;
	}
	
	HRESULT hr = E_FAIL;
	BSTR path = NULL;
	DWORD flags = 0;
	//DWORD err;
	LPKCSTRING String = AllocKCSTRINGA(Args, KCSF_INIT_CONVERT);
	LPKCCMDLINEARGV Argv = extract_cmdline(String->getLPTSTR(), _tcslen(String->getLPCTSTR()));

	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Get %d arguments\n", Argv->argc);
	if(Argv->argc > 0) {
		path = Argv->argv[0];
		if(Argv->argc == 2) {
			LPTSTR seek = Argv->argv[1];
			LPTSTR ptr;
			do {
				ptr = seek;
				if(*ptr == _T('\0'))
					break;
				while(*seek != _T('|') || *seek != _T('\0'))
					seek++;
				if(_istdigit(*ptr))
					flags |= extract_digtial(ptr DBGONLY_ARG(Client));
				else {
					for(int i =0; i< sizeof(LoadLibraryExFlagsMap) / sizeof(StringDwordPair); i++)
					{
						if(_tcsicmp(ptr, LoadLibraryExFlagsMap[i].Key))
							flags |= LoadLibraryExFlagsMap[i].Val;
					}
				}
			} while (true);
		}
		goto label_DOLOAD;
	}
	goto label_CLEAN;

label_DOLOAD:
	dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"Path: %s\nFlag: 0x%X\n", path, flags);
	if(_rpchelp_thread_continue) {
		hr = InitAsyncStateWithApc(g_asyncstates[2], (HANDLE)_rpchelp_thread_handle, ldlib_callback);
		if(hr == 0) {
			RpcTryExcept
				dbgCtrl4->Output(DEBUG_OUTPUT_PROMPT, "Run debuggee to take effort!\n");
				ldlib_data* pd = new ldlib_data();
				pd->path = SysAllocString(path);
				g_asyncstates[2].UserInfo = pd;
				if(flags)
					HlpmLoadLibraryEx(&g_asyncstates[2], pd->path, flags, &pd->err);
				else
					HlpmLoadLibrary(&g_asyncstates[2], pd->path, &pd->err);
			RpcExcept(1)
			RpcEndExcept
		}
		else {
			dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "Init state failed, code is: %x(%u)\n", hr, hr);
		}
	}
	
label_CLEAN:
	free(Argv);
	FreeKCSTRING(String);
	dbgCtrl4->Release();
	return S_OK;
}

void RPC_ENTRY detour_callback(
struct _RPC_ASYNC_STATE *pAsync,
	void *Context,
	RPC_ASYNC_EVENT Event)
{
	if(Event == RPC_ASYNC_EVENT::RpcCallComplete)
	{
		char buf[512];
		ULONG64* p = (ULONG64*)pAsync->UserInfo;
		if(p) {
			p = (ULONG64*)(*p);
			sprintf(buf, "Detour completed.\nThe identifier is: %I64p\n", p);
			ExtensionApis.lpOutputRoutine(buf);
		}
		else
			ExtensionApis.lpOutputRoutine("Detour completed.\n");

		RpcAsyncCompleteCall(pAsync, NULL);
	}
}

/*
 * Desciption:
 *    Detour a function at specified address.
 *    It works like Microsoft Detour Library.
 * Usage:
 *    !detour <target_VA_or_RVA_or_ExportedApi> <source_VA_or_RVA_or_ExportedApi>
 *    target - the function to be detoured
 *    source - the function for detouring
 *    VA - the Virtual Address, can be Hex or Dec digital
 *    RVA - the Relative Virtual Address, should be in "offset|module" format
 *    ExportedApi - exported function, should be "api_name|module" format
 * Example:
 *    !detour 0x54003212 0x54120056
 *    !detour 54003212h 0x120056|detour
 *    !detour detour|helpme detourx|helpmex
 */
HELPME_COMMAND(detour)
{
	IDebugControl4* dbgCtr4;
	Client->QueryInterface(__uuidof(IDebugControl4), (void**)&dbgCtr4);

	LPKCSTRING String = AllocKCSTRINGA(Args, KCSF_INIT_CONVERT);
	LPKCCMDLINEARGV Argv = extract_cmdline(String->getLPTSTR(), _tcslen(String->getLPCTSTR()));
	if(Argv->argc != 2)
		goto label_CLEAN;

	ULONG64 target = 0, source = 0;
	dbgCtr4->OutputWide(DEBUG_OUTPUT_NORMAL, L"argv[0] is %s\n", Argv->argv[0]);
	target = extract_address(Argv->argv[0], Client);
	if(!target) {
		dbgCtr4->OutputWide(DEBUG_OUTPUT_NORMAL, L"Extract TARGET address faild, nothing will do.\n");
		goto label_CLEAN;
	}
	dbgCtr4->OutputWide(DEBUG_OUTPUT_NORMAL, L"argv[1] is %s\n", Argv->argv[1]);
	source = extract_address(Argv->argv[1], Client);
	if(!source) {
		dbgCtr4->OutputWide(DEBUG_OUTPUT_NORMAL, L"Extract SOURCE address faild, nothing will do.\n");
		goto label_CLEAN;
	}

	dbgCtr4->Output(DEBUG_OUTPUT_NORMAL, "Detour, target: %y, source: %y\n", target, source);
	if(target && source && _rpchelp_thread_handle) {
		InitAsyncStateWithApc(g_asyncstates[3], (HANDLE)_rpchelp_thread_handle, detour_callback);
		RpcTryExcept
			g_asyncstates[3].UserInfo = malloc(sizeof(ULONG64));
			HlpmDetourAttach(&g_asyncstates[3], target, source, 0, (ULONG64*)g_asyncstates[3].UserInfo);
			dbgCtr4->Output(DEBUG_OUTPUT_PROMPT, "Run debuggee to take efforts.");
		RpcExcept(1)
		RpcEndExcept
	}
label_CLEAN:
	free(Argv);
	FreeKCSTRING(String);
	dbgCtr4->Release();
	return S_OK;
}

void RPC_ENTRY tstdetour_callback(
struct _RPC_ASYNC_STATE *pAsync,
	void *Context,
	RPC_ASYNC_EVENT Event)
{
	if(Event == RPC_ASYNC_EVENT::RpcCallComplete)
	{
		RpcAsyncCompleteCall(pAsync, NULL);
	}
}

RPC_ASYNC_STATE tststate;

HELPME_COMMAND(tstdetour)
{
	if(_rpchelp_thread_continue) {
		InitAsyncStateWithApc(tststate, (HANDLE)_rpchelp_thread_handle, tstdetour_callback);
		RpcTryExcept
			HlpmTestDetour(&tststate);
		RpcExcept(1)
		RpcEndExcept
	}
	return S_OK;
}

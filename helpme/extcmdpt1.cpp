#include "stdafx.h"
#include <string>
#include "helpme.h"

#include "../detours/detourplus.h"

#pragma comment(lib, "../Debug/detours.lib")

HELPME_COMMAND(symoff)
{
	if(Args == NULL || strlen(Args) == 0)
		return DEBUG_EXTENSION_CONTINUE_SEARCH;

	CComQIPtr<IDebugSymbols> dbgSym = Client;
	ULONG64 addr;
	HRESULT hr = dbgSym->GetOffsetByName(Args, &addr);
	if(SUCCEEDED(hr)) {
		CComQIPtr<IDebugControl> dbgCtrl = Client;
		dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "%y\n", addr);
	}
	return hr;
}

void string_impl(IDebugClient *Client, ULONG64 address, int type, int len, bool isPointer)
{
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CComQIPtr<IDebugSymbols> dbgSym = Client;
	CComQIPtr<IDebugDataSpaces> dbgData = Client;
	if(isPointer) {
		dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Read pointer\n");
		dbgData->ReadPointersVirtual(1, address, &address);
	}
	if(len == 0)
		len = 1024;
	else if(len > 1020)
		len = 1024;
	else if(len < 0)
		return;

	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "the length is %d\n", len);
	ULONG read = sizeof(int*);
	if(type == 'W')
		read = len * sizeof(wchar_t);
	else
		read = len * sizeof(char);
	BYTE *pbuf = new BYTE[read];
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "the address is %I64X, %u bytes to read\n", address, read);
	dbgData->ReadVirtual(address, pbuf, read, &read);
	pbuf[read-1] = '\0';
	pbuf[read-2] = '\0';	
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "%u bytes read, the first 16 raw bytes are:\n", read);
	for(int i=0; i<16; i++) {
		dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, " %02X", (DWORD)pbuf[i]);
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "\nDumping string at %y ...\n", address);
	if(type == 'W')
		dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"%s\n", pbuf);
	else
		dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "%s\n", pbuf);
	delete pbuf;
}

/*
 * Description:
 *     dump a string at specified address
 * Usage:
 *     !string [/mN=0|/wN=0] <VA_or_RVA> /i
 *     VA_or_RVA - Must be Hex value (prefix with '0x' or postfix with 'h')
 *                 the format is RVA|ModuleName or VA
 *     /mN|/wN - Indicate the string type( m for MBCS, w for Unicode)
 *               N is the string length want to dump, 0 mean until NUL char, 
 *               can be Hex or dec. Default to /a0, Max to 4090
 *     /i - Indicate the VA_or_RVA is a pointer point to the string
 * Example:
 *     !string /a32 0x3242|helpme
 *     !string /w 0x65840000
 *     !string 0x3243|helpme
 */
HELPME_COMMAND(string)
{
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CComQIPtr<IDebugSymbols3> dbgSym3 = Client;		

	HRESULT hr = DEBUG_EXTENSION_CONTINUE_SEARCH;
	if(Args == NULL || strlen(Args) == 0 ) {
		dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "Invalid argumets, debugger will continue search handler.\n");
		return hr;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "command args: %s\n", Args);
	DWORD flags = KCSF_INIT_NONE;
#if defined(UNICODE) || defined(_UNICODE)
	flags |= KCSF_INIT_CONVERT;
#endif
	LPKCSTRING kcstring = AllocKCSTRINGA(Args, flags);
	LPKCCMDLINEARGV Argv = extract_cmdline(kcstring->getLPTSTR(), _tcslen(kcstring->getLPCTSTR()));	
	if(!Argv) {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "extrac command failed\n");
		FreeKCSTRING(kcstring);
		return E_FAIL;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "%n command extracted\n");
	ULONG64 addr = 0;
	int len = 0;
	int type = 'M';
	bool indirect = false;
	for(int i = 0 ; i<Argv->argc; i++)
	{
		dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("arg[%u]: %s\n"), i, Argv->argv[i]);
		if(_tcsicmp(Argv->argv[i], _T("-i")) == 0)
			indirect = true;
		else if(_tcsnicmp(Argv->argv[i], _T("-m"), 2) == 0) {
			len = extract_digtial(Argv->argv[i]+2 DBGONLY_ARG(Client));
			type = 'M';
		}
		else if(_tcsnicmp(Argv->argv[i], _T("-w"), 2) == 0 ) {
			len = extract_digtial(Argv->argv[i]+2 DBGONLY_ARG(Client));
			type = 'W';
		}
		else {
			TCHAR* sp = Argv->argv[i];
			while(*sp != _T('|') && *sp != _T('\0'))
				sp++;
			if(*sp == _T('|')) {
				*sp = _T('\0'); sp++;
				dbgSym3->GetModuleByModuleNameT(sp, 0, 0, &addr);
				dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("Get base address for %s %I64p\n"), sp, addr);
			}
			addr += extract_digtial(Argv->argv[i], Client);
		}			
	}
	free(Argv);
	FreeKCSTRING(kcstring);
	if(addr) {
		string_impl(Client, addr, type, len, indirect);
	}
	else {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "Invalid address, helpme cann't do anything!\n");
	}	
	return S_OK;
}

SimpleLinked<DWORD> *gp_takachance_pids;
#define TAKACHANCEPATH _T("A:\\helpme\\debug\\TakaChance.dll")
extern UUID RpcEndpointId;

/*
 * Description:
 *     Inject TakaChance.Dll to the process
 * Usage:
 *     !takachance
 * Example:
 *     !takachance
 */
HELPME_COMMAND(takachance)
{
	HRESULT hr = E_FAIL;
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CComQIPtr<IDebugDataSpaces> dbgData = Client;
	CComQIPtr<IDebugSymbols3> dbgSym3 = Client;		
	CComQIPtr<IDebugSystemObjects> dbgSys = Client;

	// prepare LoadLibrary
	ULONG64 AddressLoadLibraryW;
	hr = dbgSym3->GetOffsetByName("LoadLibraryW", &AddressLoadLibraryW);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "LoadLibraryW: %y\n", AddressLoadLibraryW);
	ULONG64 AddressGetLastError;
	hr = dbgSym3->GetOffsetByName("GetLastError", &AddressGetLastError);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "AddressGetLastError: %y\n", AddressGetLastError);
	ULONG64 AddressGetProcAddress;
	hr = dbgSym3->GetOffsetByName("GetProcAddress", &AddressGetProcAddress);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "AddressGetProcAddress: %y\n", AddressGetProcAddress);

	// prepare process
	DWORD pid = 0;
	dbgSys->GetCurrentProcessSystemId(&pid);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "The target process id is %x(%u)\n", pid, pid);	
	if(gp_takachance_pids) {
		if(gp_takachance_pids->Search(pid)) {
			dbgCtrl4->Output(DEBUG_OUTPUT_PROMPT, "The target process has took a chance already\n", pid, pid);
			return E_FAIL;
		}
	}

	HANDLE hProcess = OpenProcess(GENERIC_ALL, FALSE, pid);
	if(hProcess) {
		dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Opened handle for process %x(%u)\n", pid, pid);
	}
	else {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "Fail to open process handle\n");
		return E_FAIL;		
	}
	
	// alloc page
	BYTE* Page = (BYTE*)VirtualAllocEx((HANDLE)hProcess, NULL, 4000, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if(!Page) {
		dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Fail to alloc memory\n");
		CloseHandle(hProcess);
		return E_FAIL;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Allocated memory at %p\n", Page);
	VirtualProtectEx(hProcess, Page, 4000, PAGE_READWRITE, &pid);

	// prepare code
	char *buf = new char[1024];
	BYTE *pData = Page;	
	DWORD Data_Dll = (DWORD)pData;
	WriteProcessMemory(hProcess, pData, TAKACHANCEPATH, sizeof(TAKACHANCEPATH), &pid);
	pData += sizeof(TAKACHANCEPATH);
	DWORD Data_Main = (DWORD)pData;
	WriteProcessMemory(hProcess, pData, "TakaChanceMain", sizeof("TakaChanceMain"), &pid);
	pData += sizeof("TakaChanceMain");
	DWORD Data_Uuid = (DWORD)pData;
	UuidCreate(&RpcEndpointId);
	WriteProcessMemory(hProcess, pData, &RpcEndpointId, sizeof(UUID), &pid);
	RPC_CSTR str;
	UuidToStringA(&RpcEndpointId, &str);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Created uuid: %s\n", str);
	RpcStringFreeA(&str);
	pData += sizeof(UUID);
	BYTE *pCode = pData;	
	ULONG64 u64 = (ULONG64)pCode;
	
	dbgCtrl4->Assemble(u64, "push ebp", &u64);
	dbgCtrl4->Assemble(u64, "mov ebp, esp", &u64);
	// push offset string "TakaChance.dll"
	sprintf_s(buf, 1024, "mov eax, 0x%p", Data_Dll); 
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "code: %s\n", buf);
	dbgCtrl4->Assemble(u64, buf, &u64); 
	dbgCtrl4->Assemble(u64, "push eax", &u64);
	// mov eax, AddressLoadLibraryA
	sprintf_s(buf, 1024, "mov eax, 0x%x", (DWORD)AddressLoadLibraryW);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "code: %s\n", buf);
	dbgCtrl4->Assemble(u64, buf, &u64);
	dbgCtrl4->Assemble(u64, "call eax", &u64);
	dbgCtrl4->Assemble(u64, "mov ecx, eax", &u64);
	// call GetLastError
	sprintf_s(buf, 1024, "mov eax, 0x%x", (DWORD)AddressGetLastError);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "code: %s\n", buf);
	dbgCtrl4->Assemble(u64, buf, &u64);
	dbgCtrl4->Assemble(u64, "call eax", &u64);
	// push offset string "TakaChanceMain"
	sprintf_s(buf, 1024, "mov eax, 0x%p", Data_Main); 
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "code: %s\n", buf);
	dbgCtrl4->Assemble(u64, buf, &u64); 
	dbgCtrl4->Assemble(u64, "push eax", &u64);
	dbgCtrl4->Assemble(u64, "push ecx", &u64);
	// mov eax, AddressGetProcAddress
	sprintf_s(buf, 1024, "mov eax, 0x%x", (DWORD)AddressGetProcAddress);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "code: %s\n", buf);
	dbgCtrl4->Assemble(u64, buf, &u64);
	dbgCtrl4->Assemble(u64, "call eax", &u64);
	// mov ecx, Data_Uuid
	sprintf_s(buf, 1024, "mov ecx, 0x%x", Data_Uuid);
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "code: %s\n", buf);
	dbgCtrl4->Assemble(u64, buf, &u64);
	dbgCtrl4->Assemble(u64, "call eax", &u64);
	dbgCtrl4->Assemble(u64, "mov esp, ebp", &u64);
	dbgCtrl4->Assemble(u64, "pop ebp", &u64);
	dbgCtrl4->Assemble(u64, "ret 4", &u64);
	VirtualProtectEx(hProcess, Page, 4000, PAGE_EXECUTE_READ, &pid);	
	delete buf;

	// create thread
	DWORD vmTid;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pCode, NULL, 0, &vmTid);
	if(!hThread) {
		dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Failed to create thread\n");
		VirtualProtectEx(hProcess, Page, 4000, PAGE_READWRITE, &pid);
		VirtualFreeEx(hProcess, Page, 0, MEM_RESERVE);
		CloseHandle(hProcess);
		return E_FAIL;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "Created thread %x(%u), %p\n", vmTid, vmTid, hThread);
	dbgCtrl4->Output(DEBUG_OUTPUT_PROMPT, "Run debuggee to take effect\n");
	CloseHandle(hThread);
	CloseHandle(hProcess);
	hProcess = NULL;
	Page = NULL;
	dbgSys->GetCurrentProcessSystemId(&pid);
	if(gp_takachance_pids)
		gp_takachance_pids->Add(new SimpleLinked<DWORD>(pid));
	else
		gp_takachance_pids = new SimpleLinked<DWORD>(pid);
	
	return hr;
}

// for IUnknown members
#define DMPVTBL_DEF_FUNCTIONS 3

HRESULT vftbl_impl(IDebugClient *Client, ULONG64 address, int count, bool indirect)
{
	CComQIPtr<IDebugControl> dbgCtrl = Client;
	CComQIPtr<IDebugSymbols> dbgSym = Client;
	CComQIPtr<IDebugDataSpaces> dbgData = Client;
	ULONG read = sizeof(int*);
	if(indirect)
		dbgData->ReadVirtual(address, &address, read, &read);
	read = count * sizeof(int*);
	int* *pbuf = new int*[count];
	dbgData->ReadVirtual(address, pbuf, read, &read);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "dumping vftable at %y ...\n", address);
	for(int i = 0; i<count; i++) {
		address = (ULONG64)*(pbuf+i);
		dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "%y\n", address);
	}
	return S_OK;
}

/*
 * Description:
 *     Dump a vftable
 * Usage:
 *     !vftbl <VA_or_RVA> [/i] [/n=3]
 *     VA_or_RVA - Must be Hex value (prefix with '0x' or postfix with 'h')
 *                 the format is RVA|ModuleName or VA
 *     /i - Indicate the Address is a pointer point to the table
 *     /n - Number of functions to dump, default to 3
 *	        (can be Hex or Dec, from 0 to 1024) 
 * Sample:
 *     !vftbl 0x57822312 /n5 /i
 *     !vftbl 822312h|helpme /n5
 */
HELPME_COMMAND(vftbl)
{
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CComQIPtr<IDebugSymbols3> dbgSym3 = Client;		

	HRESULT hr = DEBUG_EXTENSION_CONTINUE_SEARCH;
	if(Args == NULL || strlen(Args) == 0 ) {
		dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "Invalid argumets, debugger will continue search handler.\n");
		return hr;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "command args: %s\n", Args);
	DWORD flags = KCSF_INIT_NONE;
#if defined(UNICODE) || defined(_UNICODE)
	flags |= KCSF_INIT_CONVERT;
#endif
	LPKCSTRING kcstring = AllocKCSTRINGA(Args, flags);
	LPKCCMDLINEARGV Argv = extract_cmdline(kcstring->getLPTSTR(), _tcslen(kcstring->getLPCTSTR()));	
	if(!Argv) {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "extract command failed\n");
		FreeKCSTRING(kcstring);
		return E_FAIL;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "%d command extracted\n", Argv->argc);
	ULONG64 addr = 0;
	int cnt = DMPVTBL_DEF_FUNCTIONS;
	bool indirect = false;
	for(int i = 0 ; i<Argv->argc; i++)
	{
		dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("arg[%u]: %s\n"), i, Argv->argv[i]);
		if(_tcsicmp(Argv->argv[i], _T("-i")) == 0)
			indirect = true;
		else if(_tcsnicmp(Argv->argv[i], _T("-n"), 2) == 0)
			cnt = (int)extract_digtial(Argv->argv[i]+2, Client);
		else {
			TCHAR* sp = Argv->argv[i];
			while(*sp != _T('|') && *sp != _T('\0'))
				sp++;
			if(*sp == _T('|')) {
				*sp = _T('\0'); sp++;
				dbgSym3->GetModuleByModuleNameT(sp, 0, 0, &addr);
				dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("Get base address for %s %I64p\n"), sp, addr);
			}
			addr += extract_digtial(Argv->argv[i], Client);
		}			
	}
	
	free(Argv);
	FreeKCSTRING(kcstring);
	hr = E_FAIL;
	if(addr) {
		if(cnt) {
			hr = vftbl_impl(Client, addr, cnt, indirect);
		}
		else {
			dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "The count is 0, no function is dumped!\n");
			hr = S_FALSE;
		}
	}
	else {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "Invalid address, helpme cann't do anything!\n");
	}	
	return hr;
}

#define QIMAP_DEF_ENTRIES 120

HRESULT qimap_impl(IDebugClient *Client, ULONG64 address, int count, bool indirect)
{
	CComQIPtr<IDebugControl> dbgCtrl = Client;
	CComQIPtr<IDebugSymbols> dbgSym = Client;
	CComQIPtr<IDebugDataSpaces> dbgData = Client;
	if(indirect)
		dbgData->ReadPointersVirtual(1, address, &address);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "dumping QI map table at %y ...\n", address);	
	struct MapEntry{
		DWORD guid;
		DWORD offset;
		DWORD function;
	};
	ULONG read = 4096;
	int enycnt = read / sizeof(MapEntry);
	MapEntry *entries = new MapEntry[enycnt];	
	dbgData->ReadVirtual(address, entries, read, &read);
	GUID guid;
	int i = 0;
	for( ; i<enycnt && i<count; i++) {
		address = (ULONG64)entries[i].guid;
		if(address == 0)
			break;
		dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "[%y]\n", address);
		dbgData->ReadVirtual(address, &guid, sizeof(GUID), NULL);
		address = (ULONG64)entries[i].function;
		dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "-> %08X-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X, %08X(%012u), %y\n",
			guid.Data1, guid.Data2, guid.Data3, *(unsigned short*)(&guid.Data4[0]),
			guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7],
			entries[i].offset, address);		
	}
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "dumped %d entries\n", i);
	return S_OK;
}

/*
 * Description:
 *     Dump a ATL QueryInterface entry table
 * Usage:
 *     !qimap <VA_or_RVA> [/i] [/n=1024]
 *     VA_or_RVA - Must be Hex value (prefix with '0x' or postfix with 'h')
 *                 the format is RVA|ModuleName or VA
 *     /i - Indicate the Address is a pointer point to the table
 *     /n - The maximum number of entry to dump, default to 120.
 *          if the map has no so much entries, it will dump all.
 *	        
 * Sample:
 *     !qimap 0x57822312 /n5 /i
 *     !qimap 822312h|helpme /n5
 */
HELPME_COMMAND(qimap)
{
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CComQIPtr<IDebugSymbols3> dbgSym3 = Client;		

	HRESULT hr = DEBUG_EXTENSION_CONTINUE_SEARCH;
	if(Args == NULL || strlen(Args) == 0 ) {
		dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "Invalid argumets, debugger will continue search handler.\n");
		return hr;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "command args: %s\n", Args);
	DWORD flags = KCSF_INIT_NONE;
#if defined(UNICODE) || defined(_UNICODE)
	flags |= KCSF_INIT_CONVERT;
#endif
	LPKCSTRING kcstring = AllocKCSTRINGA(Args, flags);
	LPKCCMDLINEARGV Argv = extract_cmdline(kcstring->getLPTSTR(), _tcslen(kcstring->getLPCTSTR()));	
	if(!Argv) {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "extract command failed\n");
		FreeKCSTRING(kcstring);
		return E_FAIL;
	}
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "%d command extracted\n", Argv->argc);
	ULONG64 addr = 0;
	int cnt = QIMAP_DEF_ENTRIES;
	bool indirect = false;
	for(int i = 0 ; i<Argv->argc; i++)
	{
		dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("arg[%u]: %s\n"), i, Argv->argv[i]);
		if(_tcsicmp(Argv->argv[i], _T("-i")) == 0)
			indirect = true;
		else if(_tcsnicmp(Argv->argv[i], _T("-n"), 2) == 0)
			cnt = (int)extract_digtial(Argv->argv[i]+2, Client);
		else {
			TCHAR* sp = Argv->argv[i];
			while(*sp != _T('|') && *sp != _T('\0'))
				sp++;
			if(*sp == _T('|')) {
				*sp = _T('\0'); sp++;
				dbgSym3->GetModuleByModuleNameT(sp, 0, 0, &addr);
				dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("Get base address for %s %I64p\n"), sp, addr);
			}
			addr += extract_digtial(Argv->argv[i], Client);
		}			
	}

	free(Argv);
	FreeKCSTRING(kcstring);
	hr = E_FAIL;
	if(addr) {
		if(cnt) {
			hr = qimap_impl(Client, addr, cnt, indirect);
		}
		else {
			dbgCtrl4->Output(DEBUG_OUTPUT_WARNING, "The count is 0, no function is dumped!\n");
			hr = S_FALSE;
		}
	}
	else {
		dbgCtrl4->Output(DEBUG_OUTPUT_ERROR, "Invalid address, helpme cann't do anything!\n");
	}	
	return hr;
}

HELPME_COMMAND(echo)
{		
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;	
	dbgCtrl4->Output(DEBUG_OUTPUT_NORMAL, "MBCS(多字节): %s\n", Args);
	dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"(混合输出): %s\n", (LPWSTR)Args);
	LPKCSTRING kcstring = AllocKCSTRINGA(Args, KCSF_INIT_CONVERT);
	dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"WIDE(统一字): %s\n", kcstring->wide_string);
	FreeKCSTRING(kcstring);
	return S_OK;
}

BOOL __stdcall _DoWriteFile(HANDLE hFile, LPVOID buf, DWORD size, DWORD* wrote, LPOVERLAPPED overlap)
{
	return WriteFile(hFile, buf, size, wrote, overlap);

}
BOOL __stdcall _DoWriteFileStub(HANDLE hFile, LPVOID buf, DWORD size, DWORD* wrote, LPOVERLAPPED overlap)
{
	return TRUE;
}

void disasm_impl(IDebugClient *Client, ULONG64 addrBegin, ULONG64 addrEnd, HANDLE hFile)
{
	typedef BOOL (__stdcall *PfnWriteFile)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
	PfnWriteFile writeFile = _DoWriteFileStub;
	if(hFile)
		writeFile = _DoWriteFile;

	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CHAR *buf = new CHAR[1024];
	ULONG write = 0;
	do
	{
		dbgCtrl4->Disassemble(addrBegin, DEBUG_DISASM_EFFECTIVE_ADDRESS, buf, 1024, &write, &addrBegin);
		buf[write] = '\r'; write++;
		buf[write] = '\n'; write++;
		buf[write] = '\0';
		dbgCtrl4->Output(DEBUG_OUTPUT_SYMBOLS, "%s", buf);
		writeFile(hFile, buf, write, &write, NULL);
	} while (addrEnd > addrBegin);
	
	if(hFile) {
		FlushFileBuffers(hFile);	
		SetEndOfFile(hFile);		
	}
}

/*
 * Description:
 *     Disassembly specified address to output file.
 * Usage:
 *     !disasm <VA_or_RVA_start>[<VA_or_RVA_end|+Bytes] [OutputFile[*]]
 *     VA_or_RVA - Must be Hex value (prefix with '0x' or postfix with 'h')
 *                 the format is RVA|ModuleName or VA
 *     Bytes - the number of bytes to disasm
 *     OutputFile - the file to save the disasm code, 
 *                  "*" means create new or truncate the existing file.
 *                  the default is open and append the file
 *	        
 * Sample:
 *     !disasm 0x57822312
 *     !disasm 0x57822312+34
 *     !disasm 0x57822312<0x57822346
 *     !disasm 822312h|helpme
 *     !disasm 822312h|helpme+34
 *     !disasm 822312h|helpme<822346h|helpme
 *     !disasm 822312h<822346h<helpme
 *     !disasm 0x57822312<822346h|helpme
 *     !disasm 822312h|helpme<0x57822312
 */
HELPME_COMMAND(disasm)
{
	if(Args == NULL)
		return DEBUG_EXTENSION_CONTINUE_SEARCH;

	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	CComQIPtr<IDebugSymbols3> dbgSym3 = Client;

	LPKCSTRING kcstr = AllocKCSTRINGA(Args, KCSF_INIT_CONVERT);
	LPKCCMDLINEARGV Argv = extract_cmdline(kcstr->getLPTSTR(), strlen(Args));
	DBGONLY_STATEMENT(dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"argc: %d, argv[0] %s\n", Argv->argc, Argv->argv[0]);)
	
	int c_pipe = 0, c_less = 0, c_plus = 0;
	LPWSTR argv[4];
	LPWSTR ptr = Argv->argv[0];	
	argv[0] = ptr;
	int i = 1; int j = -1;
	DBGONLY_STATEMENT(dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"[E] i: %d, c_pipe: %d, c_less: %d, c_plus: %d\n", i, c_pipe, c_less, c_plus);)
	while(*ptr != L'\0' && i < 4)
	{
		if(*ptr == L'<')
		{
			*ptr = L'\0';
			ptr++;
			argv[i] = ptr;
			i++;
			c_less++;
			if(j<0) j = L'<';
		}
		else if(*ptr == L'|')
		{
			*ptr = L'\0';
			ptr++;
			argv[i] = ptr;
			i++;
			c_pipe++;
			if(j<0) j = L'|';
		}
		else if(*ptr == L'+')
		{
			*ptr = L'\0';
			ptr++;
			argv[i] = ptr;
			i++;
			c_plus++;
		}
		else
			ptr++;
	} 
	DBGONLY_STATEMENT(dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"i: %d, c_pipe: %d, c_less: %d, c_plus: %d\n", i, c_pipe, c_less, c_plus);)

	ULONG64 addr_begin, addr_end = 0;
	if(c_pipe == 0 && c_less == 0) {
		addr_begin = extract_digtial(argv[0] DBGONLY_ARG(Client));
		if(c_plus == 1)
			addr_end = addr_begin + extract_digtial(argv[1] DBGONLY_ARG(Client));
	}
	else if(c_pipe == 0 && c_less == 1) {
		addr_begin = extract_digtial(argv[0] DBGONLY_ARG(Client));
		addr_end = extract_digtial(argv[1] DBGONLY_ARG(Client));
	}
	else if(c_pipe == 1 && c_less == 0) {
		dbgSym3->GetModuleByModuleNameWide(argv[1], 0, NULL, &addr_begin);
		addr_begin += extract_digtial(argv[0] DBGONLY_ARG(Client));
		if(c_plus == 1)
			addr_end = addr_begin + extract_digtial(argv[2] DBGONLY_ARG(Client));
	}
	else if(c_pipe == 2 && c_less == 1) {
		dbgSym3->GetModuleByModuleNameWide(argv[1], 0, NULL, &addr_begin);
		addr_begin += extract_digtial(argv[0] DBGONLY_ARG(Client));
		dbgSym3->GetModuleByModuleNameWide(argv[3], 0, NULL, &addr_end);
		addr_end += extract_digtial(argv[2] DBGONLY_ARG(Client));
	}
	else if(c_pipe == 0 && c_less == 2) {
		dbgSym3->GetModuleByModuleNameWide(argv[2], 0, NULL, &addr_begin);
		addr_end = addr_begin;
		addr_begin += extract_digtial(argv[0] DBGONLY_ARG(Client));
		addr_end += extract_digtial(argv[1] DBGONLY_ARG(Client));
	}
	else if(c_pipe == 1 && c_less == 1) {
		if(j == L'<') {
			addr_begin = extract_digtial(argv[0] DBGONLY_ARG(Client));
			dbgSym3->GetModuleByModuleNameWide(argv[2], 0, NULL, &addr_end);
			addr_end += extract_digtial(argv[1] DBGONLY_ARG(Client));
		}
		else if(j == L'|') {
			dbgSym3->GetModuleByModuleNameWide(argv[1], 0, NULL, &addr_begin);
			addr_begin += extract_digtial(argv[0] DBGONLY_ARG(Client));
			addr_end = extract_digtial(argv[2] DBGONLY_ARG(Client));
		}
	}
	else
		goto label_CLEANUP;
	if(addr_end && addr_begin>addr_end)
		std::swap<ULONG64>(addr_begin, addr_end);
	HANDLE hFile = NULL;
	if(Argv->argc >= 2) {
		DWORD flag = CREATE_NEW;
		i = _tcslen(Argv->argv[1]) - 1;
		DBGONLY_STATEMENT(dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"file is: %s, %c\n", Argv->argv[1], (Argv->argv[1])[i]);)
		if((Argv->argv[1])[i] == _T('*'))
		{
			(Argv->argv[1])[i] = _T('\0');
			flag = CREATE_ALWAYS;
			DBGONLY_STATEMENT(dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"!!!Create Always!!!\n");)
		}
		hFile = CreateFile(Argv->argv[1], GENERIC_ALL, FILE_SHARE_READ, NULL, flag, FILE_ATTRIBUTE_NORMAL, NULL);
		if(GetLastError() == ERROR_FILE_EXISTS) {
			hFile = CreateFile(Argv->argv[1], GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
			SetFilePointer(hFile, 0, NULL, FILE_END);
		}
		if(hFile == INVALID_HANDLE_VALUE)
			hFile = NULL;
	}
	DBGONLY_STATEMENT(dbgCtrl4->OutputWide(DEBUG_OUTPUT_NORMAL, L"begin: %I64X, end: %I64X, hFile, %p\n", addr_begin, addr_end, hFile);)
	disasm_impl(Client, addr_begin, addr_end, hFile);
	CloseHandle(hFile);
label_CLEANUP:
	free(Argv);
	FreeKCSTRING(kcstr);
	return S_OK;
}

void asm_impl(IDebugClient* Client, LPWSTR lpAsm, ULONG64 Addr, bool Indirect)
{
	CComQIPtr<IDebugControl4> dbgCtrl = Client;
	CComQIPtr<IDebugDataSpaces> dbgData = Client;
	LPWSTR pInstruction;
	if(Indirect)
		Addr = GetIndirectAddress(dbgData, Addr);	

	do {
		pInstruction = lpAsm;
		if(*pInstruction == L'\0')
			break;
		dbgCtrl->AssembleWide(Addr, pInstruction, &Addr);
		while(*lpAsm != L'>' && *lpAsm != L'\0')
			lpAsm++;
		lpAsm = L'\0';
		lpAsm ++;
	} while(true);
}

/*
 * Description:
 *     Write specified assembly code to debugee's address
 * Usage:
 *     !asm [/i] <VA_or_RVA_start> <Instruction_List>
 *     NOTE: the args Must be in this order
 *     VA_or_RVA - Must be Hex value (prefix with '0x' or postfix with 'h')
 *                 the format is RVA|ModuleName or VA
 *     Instruction_List - Assembly code, seperated by '>'.
 *                        exp: nop>ud>push eax
 *     /i - Indicate the Address is a pointer point to the table
 *	        
 * Sample:
 *     !disasm 0x57822312 nop>nop
 *     !disasm /i 0x57822312 nop>nop
 *     !disasm 822312h|helpme nop>nop
 *     !disasm /i 822312h|helpme nop>nop
 */
HELPME_COMMAND(asml)
{
	CComQIPtr<IDebugSymbols3> dbgSym3 = Client;
	CComQIPtr<IDebugControl4> dbgCtrl4 = Client;
	LPKCSTRING String = AllocKCSTRINGA(Args, KCSF_INIT_CONVERT);
	LPKCCMDLINEARGV Argv = extract_cmdline(String->getLPTSTR(), wcslen(String->getLPCTSTR()));
	bool indirect = false;
	int i = 0;
	if(Argv->argc == 3) {
		indirect = true;  i = 1;
	}
	LPTSTR sp = Argv->argv[i];
	ULONG64 addr = 0;
	while(*sp != _T('|') && *sp != _T('\0'))
		sp++;
	if(*sp == _T('|')) {
		*sp = _T('\0'); sp++;
		dbgSym3->GetModuleByModuleNameT(sp, 0, 0, &addr);
		dbgCtrl4->OutputT(DEBUG_OUTPUT_NORMAL, _T("Get base address for %s %I64p\n"), sp, addr);
	}
	addr += extract_digtial(Argv->argv[i], Client);
	i++;	
	asm_impl(Client, Argv->argv[i], addr, indirect);
	free(Argv);
	FreeKCSTRING(String);
	return S_OK;
}

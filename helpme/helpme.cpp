
#include "stdafx.h"
#include "resource.h"
#include "helpme.h"

long CleanUpRpcResources(void);

extern SimpleLinked<DWORD> *gp_takachance_pids;
// debugger callbacks
extern "C" {
WINDBG_EXTENSION_APIS   ExtensionApis;

HRESULT CALLBACK DebugExtensionInitialize(OUT PULONG  Version, OUT PULONG  Flags)
{
	*Version = HELPME_LIBRARY_VERSION;
	*Flags = 0;
	CComPtr<IDebugClient> dbgClient;
	HRESULT hr = DebugCreate(__uuidof(IDebugClient), (void **)&dbgClient);
	if (dbgClient)
	{
		CComQIPtr<IDebugControl> dbgCtrl = dbgClient;
		if(dbgCtrl) {
			ExtensionApis.nSize = sizeof (ExtensionApis);
			hr = dbgCtrl->GetWindbgExtensionApis32(&ExtensionApis);
		}
	}
	return hr;
}

void CALLBACK DebugExtensionUninitialize(void)
{
	SimpleLinked<DWORD> *cptr = gp_takachance_pids;
	SimpleLinked<DWORD> *ptr;
	while(cptr) {
		ptr = cptr->__np;
		delete cptr;
		cptr = ptr;
	}
	gp_takachance_pids = NULL;
	CleanUpRpcResources();
}

void CALLBACK DebugExtensionNotify(IN ULONG  Notify, IN ULONG64  Argument)
{
	if(Notify == DEBUG_NOTIFY_SESSION_ACTIVE)
	{

	}
	else if(Notify == DEBUG_NOTIFY_SESSION_INACTIVE)
	{

	}
	else if(Notify == DEBUG_NOTIFY_SESSION_ACCESSIBLE)
	{

	}
	else if(Notify == DEBUG_NOTIFY_SESSION_INACCESSIBLE)
	{

	}
}

HRESULT CALLBACK KnownStructOutput(IN ULONG Flag, 
	IN ULONG64 Address, IN PSTR StructName, OUT PSTR  Buffer, IN OUT PULONG  BufferSize)
{
	if(Flag == DEBUG_KNOWN_STRUCT_GET_NAMES)
	{

	}
	else if(Flag == DEBUG_KNOWN_STRUCT_GET_SINGLE_LINE_OUTPUT)
	{

	}
	return E_NOTIMPL;
}

HELPME_COMMAND(help)
{
	CComQIPtr<IDebugControl> dbgCtrl = Client;
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "helpme windbg extension. [version: %x]\n", HELPME_LIBRARY_VERSION);
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Copyright (c) 2013 Hyper-DATA System. All Rights Reserved!\n\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    symoff <Symbol>\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        retrieve the offset of the symbol\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    string [/mN|/wN] <VA_or_RAV>\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        dump N chars at specified address\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    vftbl [/i] [/n] <VA_or_RVA>\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        dump the virtual function table at specified address\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    qimap [/i] [/n] <VA_or_RVA>\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        dump a ATL QueryInterface entry table at specified address\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    disasm <VA_or_RVA_start>[<VA_or_RVA_end|+Bytes] [OutputFile[*]]\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        disassembly specified address to output file\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    asml [/i] <VA_or_RVA_start> <Instruction_List>\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        write specified assembly code to debugee's address\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    takachance\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        load TakaChance.dll to debugee\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    atrpc\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        connect to Takachance.dll\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    dtrpc\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        disconnect from Takachance.dll\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    strpc\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        display connection state to Takachance.dll\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    ldlib <LibraryPath> [flags]\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        load specified library (dll) into debugee's address space\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "    detour <target_VA_or_RVA_or_ExportedApi> <source_VA_or_RVA_or_ExportedApi>\n");
	dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "        detour a function at specified address\n");

	return S_OK;
}
} // extern "C"

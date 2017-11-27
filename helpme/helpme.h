#ifndef ___HELPME_H_INCLUDE___
#define ___HELPME_H_INCLUDE___

#define HELPME_LIBRARY_VERSION (0x11112222UL)

#if defined(DEBUG) || defined(_DEBUG)
#define DBGONLY_PARAM(param) ,param
#define DBGONLY_ARG(arg) ,arg
#define DBGONLY_STATEMENT(...) __VA_ARGS__
#else
#define DBGONLY_PARAM(param)
#define DBGONLY_ARG(arg)
#define DBGONLY_STATEMENT(...)
#endif

#define DEBUG_ECHOMSG ExtensionApis.lpOutputRoutine
#include "helpmecommon.h"

#define KCSF_TYPE_INVALID  0x00000000ul
#define KCSF_TYPE_MBCS     0x00000001ul // string is mbcs
#define KCSF_TYPE_WIDE     0x00000003ul // string is unicode
#define KCSF_FREE          0x80000000ul // string must be freed
#define KCSF_MODE_SELF     0x10000000ul // the struct itself must be freed
#define KCSF_MODE_DATA     0x20000000ul // the struct data nust be freed
#define KCSF_IS_CONTIGIOUS 0x40000000ul // the struct and its data are in the same block
#define _KCSF_FREE_SELF    (KCSF_FREE | KCSF_MODE_SELF)
#define _KCSF_FREE_DATA    (KCSF_FREE | KCSF_MODE_DATA)
#define _KCSF_FREE_ALL     (KCSF_FREE | KCSF_MODE_SELF | KCSF_MODE_DATA)

typedef struct _KCSTRING
{
	const DWORD flags;
	union {
		const char *mbcs_string;
		const wchar_t *wide_string;
	};

#ifdef __cplusplus
	_KCSTRING(void) : flags(KCSF_TYPE_INVALID), mbcs_string(NULL) { }

	inline operator const char*(void) const { 
		if(flags & KCSF_TYPE_MBCS) return mbcs_string; 
		else return NULL;
	}
	inline operator const wchar_t*(void) const { 
		if(flags & KCSF_TYPE_WIDE) return wide_string; 
		else return NULL;
	}

	inline LPCTSTR getLPCTSTR(void) const {
#if defined(UNICODE) || defined(_UNICODE)
		if(flags & KCSF_TYPE_WIDE) return (LPCTSTR)wide_string; 
#else 
		if(flags & KCSF_TYPE_MBCS) return (LPCTSTR)mbcs_string; 
#endif
		else return NULL;
	}

	inline LPTSTR getLPTSTR(void) const {
		return (LPTSTR)getLPCTSTR();
	}
#endif
} KCSTRING, *LPKCSTRING;

void FreeKCSTRING(KCSTRING *kcstring);

#define KCSF_INIT_NONE     0x0
#define KCSF_INIT_ALLOC    0x1
#define KCSF_INIT_CONVERT (0x2 | KCSF_INIT_ALLOC)

int 
InitKCSTRINGA(
	__in const char* lpcstr, 
	__in LPKCSTRING kcstring, 
	__in DWORD flags
);

LPKCSTRING 
AllocKCSTRINGA(
	__in const char* lpcstr, 
	__in DWORD flags
);

int 
InitKCSTRINGW(
	__in const wchar_t* lpcstr, 
	__in LPKCSTRING kcstring, 
	__in DWORD flags
);

LPKCSTRING 
AllocKCSTRINGW(
	__in const wchar_t* lpcstr, 
	__in DWORD flags
);

typedef struct _KCCMDLINEARGVA
{	
	int argc;
	LPCSTR argv[1];
} KCCMDLINEARGVA, *LPKCCMDLINEARGVA;

typedef struct _KCCMDLINEARGVW
{	
	int argc;
	LPWSTR argv[1];
} KCCMDLINEARGVW, *LPKCCMDLINEARGVW;

// "/" will be replaced with "-"
LPKCCMDLINEARGVA
extract_cmdline_a(
	__in LPCSTR cmdstr, 
	__in int len
);

LPKCCMDLINEARGVW
extract_cmdline_w(
	__in LPWSTR cmdstr, 
	__in int len
);

ULONG64
extract_digtial_a(
	__in LPCSTR str 
	DBGONLY_PARAM(__in IDebugClient* Client)
);

ULONG64
extract_digtial_w(
	__in LPWSTR str 
	DBGONLY_PARAM(__in IDebugClient* Client)
);

ULONG64
extract_address_a (
	__in LPCSTR str,
	__in IDebugClient* Client
);

ULONG64
extract_address_w (
	__in LPWSTR str,
	__in IDebugClient* Client
);

#ifdef UNICODE
typedef KCCMDLINEARGVW KCCMDLINEARGV;
typedef LPKCCMDLINEARGVW LPKCCMDLINEARGV;
#define extract_cmdline extract_cmdline_w
#define extract_digtial extract_digtial_w
#define extract_address extract_address_w
#else
typedef KCCMDLINEARGVA KCCMDLINEARGV;
typedef LPKCCMDLINEARGVA LPKCCMDLINEARGV;
#define extract_cmdline extract_cmdline_a
#define extract_digtial extract_digtial_a
#define extract_address extract_address_a
#endif

#ifndef _USAGE_AS_COMMENT
#define BEGIN_COMMAND_USAGE(command) \
	void command##_usage(IDebugControl *dbgCtrl, bool verbose) {
#define COMMAND_USAGE(...) dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, __VA_ARGS__);
#define BEGIN_COMMAND_VERBOSE_USAGE() if(verbose) {
#define END_COMMAND_VERBOSE_USAGE() }
#define END_COMMAND_COMMENT() }

#define CALL_COMMAND_USAGE(command, dbgCtrl, verbose) \
	command##_usage(dbgCtrl, verbose)
#else
#define BEGIN_COMMAND_USAGE(command) /##*
#define COMMAND_USAGE(...) __VA_ARGS__
#define BEGIN_COMMAND_VERBOSE_USAGE()
#define END_COMMAND_VERBOSE_USAGE()
#define END_COMMAND_COMMENT() *##/

#define CALL_COMMAND_USAGE(command, dbgCtrl, verbose)
#endif

#define HELPME_COMMAND(extcmd) \
	HRESULT CALLBACK extcmd(__in IDebugClient *Client, __in_opt PCSTR Args)
#define HELPME_COMMAND_USAGE(extcmd) \
	void extcmd##_usage(__in IDebugControl *dbgCtrl, __in bool verbose)
#define HELPME_COMMAND_WITH_USAGE(extcmd) \
	void extcmd##_usage(__in IDebugControl *dbgCtrl, __in bool verbose); \
	HRESULT CALLBACK extcmd(__in IDebugClient *Client, __in_opt PCSTR Args)

__forceinline ULONG64 GetIndirectAddress(IDebugDataSpaces *dbgData, ULONG64 address) 
{
	dbgData->ReadPointersVirtual(1, address, &address);
	return address;
}

extern "C" {

// helpme.cpp commands
	HELPME_COMMAND(help);

// extcmdpt1.cpp commands
	HELPME_COMMAND(symoff);
	HELPME_COMMAND(string);
	HELPME_COMMAND(takachance);
	HELPME_COMMAND(vftbl);
	HELPME_COMMAND(qimap);

// extcmdpt2.cpp commands
	HELPME_COMMAND(atrpc);
	HELPME_COMMAND(dtrpc);
	HELPME_COMMAND(strpc);

}; // extern "C"

#endif

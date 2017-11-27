#include <Windows.h>
#include "hmfx.h"
#include <string>
#include <tchar.h>
#include "helpme.h"

#define SET_KCSTRING_FLAG(kcstring, flags_value) \
	*(DWORD*)(&kcstring->flags) = flags_value

void FreeKCSTRING(KCSTRING *kcstring) {
	if(kcstring) {
		if(kcstring->flags & _KCSF_FREE_ALL) {
			if((kcstring->flags & KCSF_IS_CONTIGIOUS) == 0)
				free((void*)kcstring->mbcs_string);
			free(kcstring);
			memset(kcstring, 0, sizeof(KCSTRING));
		}
		else if(kcstring->flags & _KCSF_FREE_SELF)
			free(kcstring);
		else if(kcstring->flags & _KCSF_FREE_DATA) {
			free((void*)kcstring->mbcs_string);
			memset(kcstring, 0, sizeof(KCSTRING));
		}
	}
}

int InitKCSTRINGA(const char* lpcstr, LPKCSTRING kcstring, DWORD flags) {
	int len = strlen(lpcstr) + 1;
	if(flags & KCSF_INIT_CONVERT) {		
		kcstring->wide_string = (wchar_t*)malloc(len * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, lpcstr, -1, (wchar_t*)kcstring->wide_string, len-1);
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_WIDE | _KCSF_FREE_DATA);
	}
	else if(flags & KCSF_INIT_ALLOC) {		
		kcstring->mbcs_string = (char*)malloc(len * sizeof(char));
		memcpy((void*)kcstring->mbcs_string, lpcstr, len * sizeof(char));
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_MBCS | _KCSF_FREE_DATA);
	}
	else {
		kcstring->mbcs_string = lpcstr;
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_MBCS);
	}
	return len;
}

LPKCSTRING AllocKCSTRINGA(const char* lpcstr, DWORD flags) {
	int len = strlen(lpcstr);
	ExtensionApis.lpOutputRoutine("Begin AllocKCSTRINGA ...\n");
	ExtensionApis.lpOutputRoutine("length of string at %p is %d\n", lpcstr, len);
	LPKCSTRING kcstring;
	if(flags & KCSF_INIT_CONVERT) {
		len += 2;
		ExtensionApis.lpOutputRoutine("Begin Convert ...\n");
		kcstring = (LPKCSTRING)malloc(len * sizeof(wchar_t) + sizeof(KCSTRING));	
		kcstring->wide_string = (wchar_t*)(kcstring + 1);
		ExtensionApis.lpOutputRoutine("Allocated KCSTRING at %p, string point to %p\n", kcstring, kcstring->wide_string);
		len = MultiByteToWideChar(CP_ACP, 0, lpcstr, -1, (wchar_t*)kcstring->wide_string, len-1);	
		ExtensionApis.lpOutputRoutine("%d chars converted\n", len);
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_WIDE | _KCSF_FREE_ALL | KCSF_IS_CONTIGIOUS);
	}
	else if(flags & KCSF_INIT_ALLOC) {
		len += 1;
		ExtensionApis.lpOutputRoutine("Begin Alloc ...\n");
		kcstring = (LPKCSTRING)malloc(len * sizeof(char) + sizeof(KCSTRING));
		kcstring->wide_string = (wchar_t*)(kcstring + 1);
		ExtensionApis.lpOutputRoutine("Allocated KCSTRING at %p, string point to %p\n", kcstring, kcstring->wide_string);
		memcpy((void*)kcstring->mbcs_string, lpcstr, len * sizeof(char));
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_MBCS | _KCSF_FREE_ALL | KCSF_IS_CONTIGIOUS);
	}
	else {		
		ExtensionApis.lpOutputRoutine("Begin Setvalue ...\n");
		kcstring = (LPKCSTRING)malloc(sizeof(KCSTRING));
		kcstring->mbcs_string = lpcstr;
		ExtensionApis.lpOutputRoutine("Allocated KCSTRING at %p\n", kcstring);
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_MBCS | _KCSF_FREE_SELF);
	}
	ExtensionApis.lpOutputRoutine("End AllocKCSTRINGA\n");
	return kcstring;
}

int InitKCSTRINGW(const wchar_t* lpwstr, LPKCSTRING kcstring, DWORD flags) {
	int len = wcslen(lpwstr) + 1;
	if(flags & KCSF_INIT_CONVERT) {		
		kcstring->mbcs_string = (char*)malloc(len * sizeof(wchar_t));
		BOOL B;
		len = WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, (char*)kcstring->mbcs_string, len * sizeof(wchar_t), "_", &B);
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_MBCS | _KCSF_FREE_DATA);
	}
	else if(flags & KCSF_INIT_ALLOC) {		
		kcstring->wide_string = (wchar_t*)malloc(len * sizeof(wchar_t));
		memcpy((void*)kcstring->wide_string, lpwstr, len * sizeof(wchar_t));
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_WIDE | _KCSF_FREE_DATA);
	}
	else {		
		kcstring = (LPKCSTRING)malloc(sizeof(KCSTRING));
		kcstring->wide_string = lpwstr;
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_WIDE);
	}
	return len;
}

LPKCSTRING AllocKCSTRINGW(const wchar_t* lpwstr, DWORD flags) {
	int len = wcslen(lpwstr);
	LPKCSTRING kcstring;
	if(flags & KCSF_INIT_CONVERT) {
		len += 2;
		kcstring = (LPKCSTRING)malloc(len * sizeof(wchar_t) + sizeof(KCSTRING));		
		kcstring->mbcs_string = (char*)(kcstring + 1);
		BOOL B;
		len = WideCharToMultiByte(CP_ACP, 0, lpwstr, -1, (char*)kcstring->mbcs_string, len * sizeof(wchar_t), "_", &B);
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_MBCS | _KCSF_FREE_ALL | KCSF_IS_CONTIGIOUS);
	}
	else if(flags & KCSF_INIT_ALLOC) {
		len += 1;
		kcstring = (LPKCSTRING)malloc(len * sizeof(wchar_t) + sizeof(KCSTRING));
		kcstring->wide_string = (wchar_t*)(kcstring + 1);
		memcpy((void*)kcstring->wide_string, lpwstr, len * sizeof(wchar_t));
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_WIDE | _KCSF_FREE_ALL | KCSF_IS_CONTIGIOUS);
	}
	else {
		kcstring = (LPKCSTRING)malloc(sizeof(KCSTRING));
		kcstring->wide_string = lpwstr;
		SET_KCSTRING_FLAG(kcstring, KCSF_TYPE_WIDE | _KCSF_FREE_SELF);
	}
	return kcstring;
}

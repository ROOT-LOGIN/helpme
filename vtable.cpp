#include <windows.h>
#include "detourplus.h"

#define CINT(ptr) *(int*)ptr
typedef int *VTABLE_PTR;

FUNC_PTR __stdcall ExtractVtFuncAddr(const void *pInterface, int idx)
{
	struct INTERFACE_OBJECT
	{
		VTABLE_PTR __vftable;
	};
	DWORD _eip;
	DT_STATIC_CAST_EX(const INTERFACE_OBJECT, obj, pInterface);
	BYTE *ptr = (BYTE*)(obj->__vftable[idx]);
	// check if *ptr is __asm jmp XX
	if(*ptr == 0xE9)
	{
		ptr++;
		_eip = (DWORD)(ptr + 4);
		ptr = (BYTE*)(CINT(ptr)+ _eip);
		// check if *ptr is __asm sub [esp+4], XX
		if(*(int*)ptr == 0x04246C83)
		{
			ptr+=5;
			// check if *ptr is __asm jmp XXXXXXXX
			if(*ptr == 0xE9)
			{				
				ptr++;
				_eip = (DWORD)(ptr + 4);
				ptr = (BYTE*)(CINT(ptr) + _eip);
			}
		}
		// check if *ptr is __asm jmp XXXXXXXX
		if(*ptr == 0xE9)
		{
			ptr++;
			_eip = (DWORD)(ptr + 4);
			ptr = (BYTE*)(CINT(ptr) + _eip);
		}
	}
	// check if *ptr is __asm sub [esp+4], XXXXXXXX
	else if(*(int*)ptr == 0x04246C81)
	{
		ptr+=8;
		// check if *ptr is __asm jmp XXXXXXXX
		if(*ptr == 0xE9)
		{				
			ptr++;
			_eip = (DWORD)(ptr + 4);
			ptr = (BYTE*)(CINT(ptr) + _eip);
		}
	}
	// check if *ptr is __asm sub [esp+4], XX
	else if(*(int*)ptr == 0x04246C83)
	{
		ptr+=5;
		// check if *ptr is __asm jmp XXXXXXXX
		if(*ptr == 0xE9)
		{				
			ptr++;
			_eip = (DWORD)(ptr + 4);
			ptr = (BYTE*)(CINT(ptr) + _eip);
		}
	}
	// should be the function itself, but we must ensure this
#ifndef RELEASE
	else 
		__asm int 3h
#endif
	return (FUNC_PTR)ptr;
}

int __cdecl __callproc(FUNC_PTR procedure, int argc, ...)
{
	__asm lea eax, argc
	__asm mov ecx, argc
label_PUSHARG:
	__asm cmp ecx, 0	
	__asm jle label_CALL
	__asm push [eax + ecx * 4]
	__asm dec ecx
	__asm jmp label_PUSHARG
label_CALL:
	__asm mov edx, procedure
	__asm call procedure
}

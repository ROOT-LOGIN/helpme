#ifndef ___VTABLE_H_INCLUDE___
#define ___VTABLE_H_INCLUDE___

#define GetEipAfterMe(eipval) \
	{ __asm jmp label_EipAfterMe_CALL } \
lable_EipAfterMe_GETEIP: { \
	__asm pop eax \
	__asm jmp label_EipAfterMe_RETURN } \
label_EipAfterMe_CALL: { \
	__asm xor eax, eax \
	__asm call lable_EipAfterMe_GETEIP } \
label_EipAfterMe_RETURN: { \
	__asm add eax, 6 \
	__asm mov eipval, eax }

__declspec(naked) __forceinline 
DWORD __stdcall GetEipOfNextStatement(void) {
	__asm mov eax, [esp]
	__asm add eax, 3
	__asm ret
}

#define __asm_jmp_eax() { \
	__asm cmp ebp, esp \
	__asm je label_JMPEAX \
	__asm mov esp, ebp \
	__asm pop ebp } \
label_JMPEAX: { __asm jmp eax }

// if the stub is __thiscall, we need fix up stack
#define __fix_stack(__this) {\
	/* ebp has been pushed, save 'this pointer' to edx */ \
	__asm mov edx, [ebp+4] \
	/* fix eip */ \
	__asm mov eax, [ebp+8] \
	__asm mov [ebp+4], eax \
	__asm lea eax, __this \
	__asm sub eax, ebp \
	/* from thiscall to stdcall, 'this pointer" is on stack */ \
	__asm cmp eax, 8 \
	__asm je label_FIX_STACK_SETTHIS \
	/* fix cs */ \
	__asm mov eax, [ebp+12] \
	__asm mov [ebp+8], eax } \
label_FIX_STACK_SETTHIS: { \
	/* set 'this pointer' */ \
	__asm mov __this, edx }

#define __thunk_from_thiscall(target) { \
	__asm mov ecx, this \
	__asm mov eax, target \
	__asm mov eax, [ecx + eax] \
	__asm cmp ebp, esp \
	__asm je label_CALL_FROM_THISCALL_JMP \
	__asm mov esp, ebp \
	__asm pop ebp } \
label_CALL_FROM_THISCALL_JMP: { \
	__asm push ecx \
	__asm jmp eax }

#define __thunk_from_stdcall(target) { \
	__asm mov ecx, this \
	__asm mov eax, target \
	__asm mov eax, [ecx + eax] \
	__asm cmp ebp, esp \
	__asm je label_CALL_FROM_THISCALL_JMP \
	__asm mov esp, ebp \
	__asm pop ebp } \
label_CALL_FROM_THISCALL_JMP: { \
	__asm jmp eax }

typedef int *FUNC_PTR;

EXTERN_C 
FUNC_PTR __stdcall
ExtractVtFuncAddr(
	const void *pInterface,
	int idx
);

EXTERN_C
int __cdecl
__callproc(
	FUNC_PTR procedure,
	int argc,
	...
);


#ifdef __cplusplus

template<typename TyFunc>
__forceinline TyFunc ExtractVtFuncAddrT(const void *pInterface, int idx)
{
	return (TyFunc)ExtractVtFuncAddr(pInterface, idx);
}

#endif

/* 
 * Macros for classes that has no virtual function 
 */

#define VTFUNC_ENTRY(ReturnType, CallConv, Name, EntryIndex, ...) \
	ReturnType CallConv Name(__VA_ARGS__) { {\
	__asm mov eax, this \
	__asm mov eax, [eax] \
	__asm mov edx, EntryIndex \
	__asm imul edx, 4 \
	__asm mov eax, [eax + edx] } \
	__asm_jmp_eax(); \
}

#define VTFUNC_STDCALL_ENTRY(ReturnType, Name, EntryIndex, ...) \
	VTFUNC_ENTRY(ReturnType, __stdcall, Name, EntryIndex, __VA_ARGS__) 

#define VTFUNC_THISCALL_ENTRY(ReturnType, Name, EntryIndex, ...) \
	VTFUNC_ENTRY(ReturnType, __thiscall, Name, EntryIndex, __VA_ARGS__) 

/* 
 * Macros for classes that has its own virtual functions
 */

#define DECLARE_VFTBLPTR() unsigned __vftblptr;
#define INIT_VFTBLPTR(value) __vftblptr = (unsigned)value

#define VTFUNC_ENTRY_EX(ReturnType, CallConv, Name, EntryIndex, ...) \
	ReturnType CallConv Name(__VA_ARGS__) { {\
	__asm mov eax, this \
	__asm add eax, __vftblptr \
	__asm mov eax, [eax] \
	__asm mov edx, EntryIndex \
	__asm imul edx, 4 \
	__asm mov eax, [eax + edx] } \
	__asm_jmp_eax(); \
}

#define VTFUNC_STDCALL_ENTRY2(ReturnType, Name, EntryIndex, ...) \
	VTFUNC_ENTRY_EX(ReturnType, __stdcall, Name, EntryIndex, __VA_ARGS__) 

#define VTFUNC_THISCALL_ENTRY2(ReturnType, Name, EntryIndex, ...) \
	VTFUNC_ENTRY_EX(ReturnType, __thiscall, Name, EntryIndex, __VA_ARGS__) 

#endif // ___VTABLE_H_INCLUDE___

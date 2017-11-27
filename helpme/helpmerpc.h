

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Jul 06 10:43:03 2013
 */
/* Compiler settings for helpme.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __helpmerpc_h__
#define __helpmerpc_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_helpme_0000_0000 */
/* [local] */ 

#pragma comment(lib, "rpcrt4.lib")
#if defined(UNICODE) || defined(_UNICODE)
#define RPC_TSTR RPC_WSTR
#else
#define RPC_TSTR RPC_CSTR
#endif
#define HELPME_RPC_ENDPOINT_FORMATSRING _T("helpme::[%s]")


extern RPC_IF_HANDLE __MIDL_itf_helpme_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_helpme_0000_0000_v0_0_s_ifspec;

#ifndef __IHelpmeMain_INTERFACE_DEFINED__
#define __IHelpmeMain_INTERFACE_DEFINED__

/* interface IHelpmeMain */
/* [implicit_handle][version][uuid] */ 

/* [async] */ void  HlpmConnect( 
    /* [in] */ PRPC_ASYNC_STATE HlpmConnect_AsyncHandle,
    /* [in] */ REFGUID rguidService,
    /* [out] */ GUID *pguidId);

/* [async] */ void  HlpmDisconnect( 
    /* [in] */ PRPC_ASYNC_STATE HlpmDisconnect_AsyncHandle,
    /* [in] */ REFGUID rguidId);


extern handle_t HelpmeMain_IfHandle;


extern RPC_IF_HANDLE IHelpmeMain_v1111_2222_c_ifspec;
extern RPC_IF_HANDLE IHelpmeMain_v1111_2222_s_ifspec;
#endif /* __IHelpmeMain_INTERFACE_DEFINED__ */

#ifndef __IHelpmeDetour_INTERFACE_DEFINED__
#define __IHelpmeDetour_INTERFACE_DEFINED__

/* interface IHelpmeDetour */
/* [implicit_handle][version][uuid] */ 

/* [async] */ void  HlpmLoadLibrary( 
    /* [in] */ PRPC_ASYNC_STATE HlpmLoadLibrary_AsyncHandle,
    /* [in] */ BSTR bstrPath,
    /* [out] */ DWORD *pdwErr);

/* [async] */ void  HlpmLoadLibraryEx( 
    /* [in] */ PRPC_ASYNC_STATE HlpmLoadLibraryEx_AsyncHandle,
    /* [in] */ BSTR bstrPath,
    /* [in] */ DWORD dwFlags,
    /* [out] */ DWORD *pdwErr);

/* [async] */ void  HlpmDetourAttach( 
    /* [in] */ PRPC_ASYNC_STATE HlpmDetourAttach_AsyncHandle,
    /* [in] */ ULONG64 pfnTarget,
    /* [in] */ ULONG64 pfnSource,
    /* [in] */ DWORD dwThreadId,
    /* [out] */ ULONG64 *Id);

/* [async] */ void  HlpmDetourDetach( 
    /* [in] */ PRPC_ASYNC_STATE HlpmDetourDetach_AsyncHandle,
    /* [in] */ ULONG64 Id);

/* [async] */ void  HlpmTestDetour( 
    /* [in] */ PRPC_ASYNC_STATE HlpmTestDetour_AsyncHandle);


extern handle_t HelpmeDetour_IfHandle;


extern RPC_IF_HANDLE IHelpmeDetour_v1111_2222_c_ifspec;
extern RPC_IF_HANDLE IHelpmeDetour_v1111_2222_s_ifspec;
#endif /* __IHelpmeDetour_INTERFACE_DEFINED__ */


#ifndef __helpmeLib_LIBRARY_DEFINED__
#define __helpmeLib_LIBRARY_DEFINED__

/* library helpmeLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_helpmeLib;
#endif /* __helpmeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



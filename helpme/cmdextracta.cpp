#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif

#include <atlbase.h>
#include <atlcom.h>

#include "hmfx.h"

#include <stdio.h>
#include <string>
#include <tchar.h>
#include "helpme.h"

#define ISCNTRL_REPLACE_CHAR _T('?')

LPKCCMDLINEARGV extract_cmdline(char* cmdstr, int len)
{
	LPKCCMDLINEARGV _ret;
	char *pp, *pn;
	int deep = 0;
	pp = cmdstr; pn = cmdstr;

	int argc = 0;
	if(cmdstr == NULL) return NULL;

	while(pn-cmdstr <= len)
	{
		if(*pn == _T('"'))
		{
			if(deep > 0) deep--;
			else deep++;
		}
		if(*pn != _T('\0'))
		{
			if(_istcntrl(*pn)) // never execute control chars
			{
				if(*pn == _T('\r') || *pn == _T('\n'))
					*pn = _T('\0');
				else
					*pn = ISCNTRL_REPLACE_CHAR;
			}
			else if(_istspace(*pn) && deep == 0)
				*pn = _T('\0');
		}
		if((*pn == _T('\0')) && (*pn != *pp))
		{
			argc++;
		}
		pp = pn++;
	}
	*pn = _T('\0');

	_ret = (LPKCCMDLINEARGV)malloc(sizeof(KCCMDLINEARGV)+sizeof(char*) * argc);
	_ret->argc = argc;
	_ret->argv[argc] = NULL;
	pn = cmdstr;
	for(argc = 0; argc<_ret->argc; argc++)
	{
		pp = pn;
		while(*pn != _T('\0')) pn++;

		_ret->argv[argc] = pp;
		//uniform command arg prefix
		if(*pp == _T('/'))
			*pp = _T('-');
		pp = pn++;
		while((*pn == _T('\0')) && (*pn == *pp))
		{
			pn++; pp++;
		}
	}
	return _ret;
}

ULONG64 extract_digtial(char* str DBGONLY_PARAM(IDebugClient* Client))
{
	CComQIPtr<IDebugControl4> dbgCtrl = Client;
	DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "str is: %s", str);)

	int len = _tcslen(str);	
	if(len < 1) {
		DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_ERROR, ", empty string!\n");)
			return 0ull;
	}

	DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, ", len is: %d\n", len);)
		int base = 10;
	int j = _totlower(str[0]);
	if(j == '0') {
		DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "begin with base 8\n");)
			base = 8;
		if(str[1] == _T('x') || str[1] == _T('X')) {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 16(by x)\n");)
				base = 16;
			if(str[len-1] == _T('h') || str[len-1] == _T('H')) {
				DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "modify last char(h->NUL)\n");)
					str[len-1] = _T('\0');
			}
		}
		else if(str[len-1] == _T('h') || str[len-1] == _T('H')) {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 16(by h), and modify last char(h->NUL)\n");)
				base = 16;
			str[len-1] = _T('\0');
		}
		else {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "checking per char\n");)
				for(int i=1; i<len; i++) {
					j = _totlower(str[i]);
					if(j == _T('8') || j == _T('9')) {
						if(base == 16) {
							DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 16(by xdigit, form 10)\n");)
								break;
						}
						else {
							DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 10(by decdigit)\n");)
								base = 10;
						}
					}
					else if(j >= _T('a') && j <= _T('f')) {
						DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 16(by hexdigit)\n");)
							base = 16;
						break;
					}
				}
		}
	}
	else if(j >= _T('1') && j <= _T('9'))
	{
		DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "begin with base 10\n");)
			base = 10;
		if(str[len-1] == _T('h') || str[len-1] == _T('H')) {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 16(by h), and modify last char(h->NUL)\n");)
				base = 16;
			str[len-1] = _T('\0');
		}
		else {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "checking per char\n");)
			for(int i=1; i<len; i++) {
				j = _totlower(str[i]);
				if(j >= _T('a') && j <= _T('f')) {
					DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base change to 16(by hexdigit)\n");)
						base = 16;
					break;
				}
			}
		}
	}
	else if(j >= _T('a') && j <= _T('f')) {
		DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "begin with base 16\n");)
			base = 16;
		j = _totlower(str[len-1]);
		if(j == _T('h')) {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "modify last char(h->NUL)\n");)
				str[len-1] = _T('\0');
		}
	}		
	ULONG64 val = std::stoull(str, 0, base);	
	DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "base is: %d, value is: %I64X(%I64u)\n", base, val, val);)
		return val;
}

// VA: 0x12345678
// RVA: helpme<!_or_|>0x23454
// SYM: helpme<!_or_|>function
ULONG64 extract_address(char* str, IDebugClient* Client)
{
	CComQIPtr<IDebugDataSpaces> dbgData = Client;
	CComQIPtr<IDebugSymbols3> dbgSym = Client;
	DBGONLY_STATEMENT(CComQIPtr<IDebugControl4> dbgCtrl = Client;)

	ULONG64 offset;
	char* ptr = str;
	while(*ptr != _T('\0') && *ptr != _T('|') && *ptr != _T('!'))
		ptr++;
	DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "The input address is: %s, got symbol pointer: %s\n", str, ptr);)

	if(*ptr == _T('0')) {
		DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Neither '!' nor '|' are found, treat as digital address format\n");)
		offset = extract_digtial(str DBGONLY_ARG(Client));
	}
	else {
		DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Either '!' or '|' is found, treat as symbol address format\n");)
		if(_istdigit(*(ptr+1))) {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "The first character for symbol ptr is %c, tereat as digital address format.\n", *(ptr+1));)
			*ptr = _T('\0');
			dbgSym->GetModuleByModuleName(str, 0, NULL, &offset);
			offset += extract_digtial(ptr+1 DBGONLY_ARG(Client));
			*ptr = _T('!');
		}
		else {
			DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "Treat as symbol address format.\n");)
			ULONG64 shan;
			*ptr = _T('!');
			dbgSym->StartSymbolMatch(str, &shan);
			dbgSym->GetNextSymbolMatch(shan, NULL, 0, NULL, &offset);
			dbgSym->EndSymbolMatch(shan);
		}
	}
	DBGONLY_STATEMENT(dbgCtrl->Output(DEBUG_OUTPUT_NORMAL, "The extracted address is %y\n", offset);)
	return offset;
}

#ifndef ___HYPERDATA_H_INCLUDE___
#define ___HYPERDATA_H_INCLUDE___

// currently, we only support x64 and x86
#if defined(ARCHIX64)
#define __archx64
#elif defined(ARCHIX86)
#define __archx86
#elif
#error Must #define and only #define one of ARCHIX64 or ARCHIX86!
#endif

typedef unsigned __int8 byte;
typedef unsigned __int16 word;
typedef unsigned __int32 dword;
typedef unsigned __int64 qword;

/*// the suffix
       - legacy mode
L      - long mode
l      - 64bit mode
A      - PAE
S      - PSE
*/

#if defined(__archx86)

#elif


#endif

#define __readonly
#define __archi_limit

#define BIT_FIELDSW(type, name) type name
#define BIT_FIELDS(type, name, b) type name : b
#define BIT_FIELD(type, name) BIT_FIELDS(type, name, 1)

#define RESR_X(type, b) BIT_FIELDS(type, _reserved_##__LINE__, b)
#define RESR_(type) RESR_X(type, 1)
#define RESR_W(type) type _reserved_##__LINE__

#define RESR_RAZX(type, b) BIT_FIELDS(type, _reserved_raz_##__LINE__, b)
#define RESR_RAZ(type) RESR_RAZX(type, 1)
#define RESR_RAZW(type) type _reserved_raz_##__LINE__

#define RESR_RAOX(type, b) BIT_FIELDS(type, _reserved_rao_##__LINE__, b)
#define RESR_RAO(type) RESR_RAOX(type, 1)
#define RESR_RAOW(type) type _reserved_rao_##__LINE__

#define RESR_MBZX(type, b) BIT_FIELDS(type, _reserved_mbz_##__LINE__, b)
#define RESR_MBZ(type) RESR_MBZX(type, 1)
#define RESR_MBZW(type) type _reserved_mbz_##__LINE__

#define RESR_MBOX(type, b) BIT_FIELDS(type, _reserved_mbo_##__LINE__, b)
#define RESR_MBO(type) RESR_MBOX(type, 1)
#define RESR_MBOW(type) type _reserved_mbo_##__LINE__

#define IGN_X(type, b) BIT_FIELDS(type, _ignore_##__LINE__, b)
#define IGN_(type) IGN_X(type, 1)
#define IGN_W(type) type _ignore_##__LINE__

#define __declstruct(name) _tag##name
#define __declptr(name) *LP##name

typedef struct _tagEFLAGS {
    BIT_FIELD(dword, CF); // carry
    RESR_RAO(dword);
    BIT_FIELD(dword, PF); // parity
    RESR_RAZ(dword);
    BIT_FIELD(dword, AF); // auxiliary
    RESR_RAZ(dword);
    BIT_FIELD(dword, ZF); // zero
    BIT_FIELD(dword, SF); // sign
    BIT_FIELD(dword, TF); // trap
    BIT_FIELD(dword, IF); // interrupt
    BIT_FIELD(dword, DF); // direction
    BIT_FIELD(dword, OF); // overflow
    BIT_FIELDS(dword, IOPL, 2); // io privilege level
    BIT_FIELD(dword, NT); // nested task
    RESR_RAZ(dword);
    BIT_FIELD(dword, RF); // resume
    BIT_FIELD(dword, VM); // vt-8086
    BIT_FIELD(dword, AC); // alignment check
    BIT_FIELD(dword, VIF); // virtual interrupt
    BIT_FIELD(dword, VIP); // virtual interrupt pending
    BIT_FIELD(dword, ID); // id
    RESR_RAZX(dword, 10);
} EFLAGS, *LPEFLAGS;

typedef struct _tagRFLAGS {
    BIT_FIELD(qword, CF); // carry
    RESR_RAO(qword);
    BIT_FIELD(qword, PF); // parity
    RESR_RAZ(qword);
    BIT_FIELD(qword, AF); // auxiliary
    RESR_RAZ(qword);
    BIT_FIELD(qword, ZF); // zero
    BIT_FIELD(qword, SF); // sign
    BIT_FIELD(qword, TF); // trap
    BIT_FIELD(qword, IF); // interrupt
    BIT_FIELD(qword, DF); // direction
    BIT_FIELD(qword, OF); // overflow
    BIT_FIELDS(qword, IOPL, 2); // io privilege level
    BIT_FIELD(qword, NT); // nested task
    RESR_RAZ(qword);
    BIT_FIELD(qword, RF); // resume
    BIT_FIELD(qword, VM); // vt-8086
    BIT_FIELD(qword, AC); // alignment check
    BIT_FIELD(qword, VIF); // virtual interrupt
    BIT_FIELD(qword, VIP); // virtual interrupt pending
    BIT_FIELD(qword, ID); // id
    RESR_RAZX(qword, 42);
} RFLAGS, *LPRFLAGS;

typedef struct _tagEFER {
    BIT_FIELD(qword, SCE); // system call extensions
    RESR_RAZX(qword, 7);
    BIT_FIELD(qword, LME); // long mode enabled
    RESR_MBZ(qword);
    BIT_FIELD(qword, LMA); // long mode active
    BIT_FIELD(qword, NXE); // no-execute enable
    union{
        BIT_FIELD(qword, SVME); // secure vm enable
        BIT_FIELD(qword, VMXE); //
    };
    BIT_FIELD(qword, LMSLE); // long mode segment limit
    BIT_FIELD(qword, FFXSR); // fast FXSAVE/FXRSTOR enable
    RESR_MBZX(qword, 49); 
} EFER, *LPEFER;

typedef union _tagCR0
{
    struct {
        BIT_FIELD(dword, PE); // protected mode
        BIT_FIELD(dword, MC); // monitor corprocessor
        BIT_FIELD(dword, EM); // emulation
        BIT_FIELD(dword, TS); // task switched
        __readonly BIT_FIELD(dword, ET); // extension type
        BIT_FIELD(dword, NE); // numeric error
        RESR_X(dword, 10);
        BIT_FIELD(dword, WP); // wtite protect
        RESR_(dword);
        BIT_FIELD(dword, AM); // alignment mask
        RESR_X(dword, 10);
        BIT_FIELD(dword, NW); // not write-though
        BIT_FIELD(dword, CD); // cache disable
        BIT_FIELD(dword, PG); // paging
    } stval;

    dword dval;
} CR0, *LPCRO;

typedef union _tagCR0L
{
    struct {
        BIT_FIELD(qord, PE); // protected mode
        BIT_FIELD(qword, MC); // monitor corprocessor
        BIT_FIELD(qword, EM); // emulation
        BIT_FIELD(qword, TS); // task switched
        __readonly BIT_FIELD(qword, ET); // extension type
        BIT_FIELD(qword, NE); // numeric error
        RESR_X(qword, 10);
        BIT_FIELD(qword, WP); // wtite protect
        RESR_(qword);
        BIT_FIELD(qword, AM); // alignment mask
        RESR_X(qword, 10);
        BIT_FIELD(qword, NW); // not write-though
        BIT_FIELD(qword, CD); // cache disable
        BIT_FIELD(qword, PG); // paging
        RESR_X(qword, 32);
    } stval;

    struct {
        dword dwlo;
        dword dwhi;
    };

    qword qval;
} CR0L, *LPCROL;

typedef struct _tagCR2
{
    dword pagefaultva;
} CR2, *LPCR2;

typedef struct _tagCR2L
{
    qword pagefaultva;
} CR2L, *LPCR2L;

// legacy Non-PAE
typedef struct _tagCR3
{
    RESR_X(dword, 3);
    BIT_FIELD(dword, PWT);
    BIT_FIELD(dword, PCD);
    RESR_X(dword, 7);
    BIT_FIELDS(dword, PDE, 20); // page directory table base address(entry)
} CR3, *LPCR3;

// legacy PAE
typedef struct _tagCR3A
{
    RESR_X(dword, 3);
    BIT_FIELD(dword, PWT);
    BIT_FIELD(dword, PCD);
    BIT_FIELDS(dword, PDPE, 27); // page directory pointer table base address(entry)
} CR3A, *LPCR3A;

// long mode
typedef struct _tagCR3L
{
    RESR_X(qword, 3);
    BIT_FIELD(qword, PWT);
    BIT_FIELD(qword, PCD);
    RESR_X(qword, 7);
    __archi_limit BIT_FIELDS(qword, PML4E, 40); // page map lvl4 table base address(entry)
    RESR_MBZX(qword, 12);
} CR3L, *LPCR3L;

typedef struct _tagCR4
{
    BIT_FIELD(dword, VME); // virtual 8086 mode extensions
    BIT_FIELD(dword, PVI); // protected mode virtual interrupts
    BIT_FIELD(dword, TSD); // timestamp disable
    BIT_FIELD(dword, DE); // debug extensions
    BIT_FIELD(dword, PSE); // page size extensions
    BIT_FIELD(dword, PAE); // physical address extension
    BIT_FIELD(dword, MCE); // machine check enable
    BIT_FIELD(dword, PGE); // page global enable
    BIT_FIELD(dword, PCE); // performance monitoring counter enable
    BIT_FIELD(dword, OSFXSR); // os FXSAVE/FXRSTOR support
    BIT_FIELD(dword, OSXUMEXCEPT); // os unmasked exception support
    RESR_MBZX(dword, 21);
} CR4, *LPCR4;

typedef struct _tagCR4L
{
    BIT_FIELD(qword, VME); // virtual 8086 mode extensions
    BIT_FIELD(qword, PVI); // protected mode virtual interrupts
    BIT_FIELD(qword, TSD); // timestamp disable
    BIT_FIELD(qword, DE); // debug extensions
    BIT_FIELD(qword, PSE); // page size extensions
    BIT_FIELD(qword, PAE); // physical address extension
    BIT_FIELD(qword, MCE); // machine check enable
    BIT_FIELD(qword, PGE); // page global enable
    BIT_FIELD(qword, PCE); // performance monitoring counter enable
    BIT_FIELD(qword, OSFXSR); // os FXSAVE/FXRSTOR support
    BIT_FIELD(qword, OSXUMEXCEPT); // os unmasked exception support
    RESR_MBZX(qword, 53);
} CR4L, *LPCR4L;

// only in 64bit mode
typedef struct _tagCR8
{

} CR8, *LPCR8;
typedef _tagCR8 TPR, *LPTPR;

typedef struct _tagSYSCFG
{
    RESR_X(dword, 18);
    BIT_FIELD(dword, MFDE); // enables use of the RdMem and WrMem attributes 
                            // in the fixed-range MTRR
    BIT_FIELD(dword,); // allows software to read and write the RdMem and WrMem bits
    BIT_FIELD(dword,); // enables the TOP_MEM register and the variable-range IORRs
    BIT_FIELD(dword,); // enables the TOP_MEM2 register
    RESR_X(dword, 10);
} SYSCFG, *LPSYSCFG;

struct _tagSegmentSelector
{
    BIT_FIELDS(word, RPL, 2); // requestor privilege level
    BIT_FIELD(word, TI); // table indicator
    BIT_FIELDS(word SI); // selector index
};
#define TI_GDT 0
#define TI_LDT 1

struct _tagSegmentRegister
{
    dword segbaseaddr;
    dword seglimit;
    word segattrib;
    _tagSegmentRegister selector;
};

// only for FS and GS in 64bit mode
struct _tagSegmentRegisterl
{
    qword segbaseaddr;
    dword seglimit;
    word segattrib;
    _tagSegmentRegister selector;
};

typedef _tagSegmentRegister CS;
typedef _tagSegmentRegister SS;
typedef _tagSegmentRegister DS;
typedef _tagSegmentRegister ES;
typedef _tagSegmentRegister FS;
typedef _tagSegmentRegister GS;

struct _tagGIDescriptorTableRegister
{
    dword dtbaseaddr;
    word dtlimit;
};
typedef _tagGIDescriptorTableRegister GDTR, *LPGDTR;
typedef _tagGIDescriptorTableRegister IDTR, *LPIDTR;

struct _tagGIDescriptorTableRegisterL
{
    qword dtbaseaddr;
    word dtlimit;
};
typedef _tagGIDescriptorTableRegister GDTRL, *LPGDTRL;
typedef _tagGIDescriptorTableRegister IDTRL, *LPIDTRL;

struct _tagLDescriptorTableRegister
{
    dword dtbaseaddr;
    dword dtlimit;
    word dtattrib;
    word dtselector;
};
typedef _tagLDescriptorTableRegister LDTR, *LPLDTR;

struct _tagLDescriptorTableRegisterL
{
    qword dtbaseaddr;
    dword dtlimit;
    word dtattrib;
    word dtselector;
};
typedef _tagLDescriptorTableRegister LDTRL, *LPLDTRL;

struct _tagGenericSegmentDescriptor
{
    BIT_FIELDS(dword, seglimit15, 16); // segment limit 0-15
    BIT_FIELDS(dword, segbaseaddr15, 16); // base address 0-15
    BIT_FIELDS(dword, segbaseaddr23, 8); // base address 16-23
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2); // descriptor privilege level
    // present, indicates whether the segment referenced by the descriptor 
    // is loaded in memory, OS set, processor Never modify, if 0 #NP
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, seglimit19, 4); // segment limit 16-19
    BIT_FIELDS(dword, AVL); // avilable to software, processor Never modify
    RESR_(dword);
    BIT_FIELD(dword, BorD); // default operand size
    BIT_FIELD(dword, G); // granularity, 1 means 4kb
    BIT_FIELDS(dword, segbaseaddr31, 8); // base address 24-31
};
#define DT_S_LDT 0u // ldt 
#define DT_S_TSS 0u // tss
#define DT_S_GAT 0u // gate
#define DT_S_COD 1u // code
#define DT_S_DAT 1u // data

inline dword getSegmentLimit(const _tagGenericSegmentDescriptor* pgd)
{
    return (pgd->seglimit15 | (pgd->seglimit19 << 16)) & 0x000FFFFFlu;
}

inline dword getSegmentBaseAddr(const _tagGenericSegmentDescriptor* pgd)
{
    return pgd->segbaseaddr15 | (pgd->segbaseaddr23 << 16) | (pgd->segbaseaddr31<<24);
}

struct _tagCodeSegmentDescriptor
{
    BIT_FIELDS(dword, seglimit15, 16);
    BIT_FIELDS(dword, segbaseaddr15, 16);
    BIT_FIELDS(dword, segbaseaddr23, 8);
    BIT_FIELD(dword, A); // accessed, set by processsor after loaded to cs, clear by os
    BIT_FIELD(dword, R); // readable, also data segment
    BIT_FIELD(dword, C); // setting to 1 identifies the code segment as conforming
    RESR_MBO(dword);
    BIT_FIELD(dword, S); // DT_S_COD
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, seglimit19, 4);
    BIT_FIELDS(dword, AVL);
    RESR_(dword);
    BIT_FIELD(dword, BorD); // D, 0 - 16bits, 1 - 32bits;
    BIT_FIELD(dword, G);
    BIT_FIELDS(dword, segbaseaddr31, 8);
};

#define DT_Type_COD_E    0x8u
#define DT_Type_COD_EA   0x9u
#define DT_Type_COD_ER   0xAu
#define DT_Type_COD_ERA  0xBu
#define DT_Type_COD_CE   0xCu
#define DT_Type_COD_CEA  0xDu
#define DT_Type_COD_CER  0xEu
#define DT_Type_COD_CERA 0xFu


struct _tagDataSegmentDescriptor
{
    BIT_FIELDS(dword, seglimit15, 16);
    BIT_FIELDS(dword, segbaseaddr15, 16);
    BIT_FIELDS(dword, segbaseaddr23, 8);
    BIT_FIELD(dword, A); // accessed
    BIT_FIELD(dword, W); // writable
    BIT_FIELD(dword, E); // expand down
    RESR_MBZ(dword);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, seglimit19, 4);
    BIT_FIELDS(dword, AVL);
    RESR_(dword);
    BIT_FIELD(dword, BorD);
    BIT_FIELD(dword, G);
    BIT_FIELDS(dword, segbaseaddr31, 8);
};

#define DT_Type_DAT_R     0x0u
#define DT_Type_DAT_RA    0x1u
#define DT_Type_DAT_RW    0x2u
#define DT_Type_DAT_RWA   0x3u
#define DT_Type_DAT_ER    0x4u
#define DT_Type_DAT_ERA   0x5u
#define DT_Type_DAT_ERW   0x6u
#define DT_Type_DAT_ERWA  0x7u

#define DT_Type_Sys_TSS16A             0x1u
#define DT_Type_Sys_LDT                0x2u
#define DT_Type_Sys_TSS16B             0x3u
#define DT_Type_Sys_CALLGATE16         0x4u
#define DT_Type_Sys_TASKGATE           0x5u
#define DT_Type_Sys_INTERRUPTGATE16    0x6u
#define DT_Type_Sys_TRAPGATE16         0x7u
#define DT_Type_Sys_TSS32A             0x9u
#define DT_Type_Sys_TSS32B             0xBu
#define DT_Type_Sys_CALLGATE32         0xCu
#define DT_Type_Sys_INTERRUPTGATE32    0xEu
#define DT_Type_Sys_TRAPGATE32         0xFu

struct _tagTSSLDTDescriptor
{
    BIT_FIELDS(dword, seglimit15, 16);
    BIT_FIELDS(dword, segbaseaddr15, 16);
    BIT_FIELDS(dword, segbaseaddr23, 8);
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, seglimit19, 4);
    BIT_FIELD(dword, AVL);
    IGN_X(dword, 2);
    BIT_FIELD(dword, G);
    BIT_FIELDS(dword, segbaseaddr31, 8);
};

struct _tagCallGateDescriptor
{
    BIT_FIELDS(dword, tgtsegoffset15, 16); // target code segment offset 0-15
    BIT_FIELDS(dword, tgtsegselector, 16); // target code segment selector
    BIT_FIELDS(dword, paramcount, 5); // parameter count
    IGN_X(dword, 3);
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, tgtsegoffset31, 16); // target code segment offset 16-31
};

inline dword getCallGateTargetSegmentOffset(const _tagCallGateDescriptor* pcgd)
{
    return pcgd->codsegoffset15 | (pcgd->codsegoffset31 << 16);
}

struct _tagInterruptTrapGateDescriptor
{
    BIT_FIELDS(dword, tgtsegoffset15, 16); // target code segment offset 0-15
    BIT_FIELDS(dword, tgtsegselector, 16); // target code segment selector
    IGN_X(dword, 5);
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, tgtsegoffset31, 16); // target code segment offset 16-31
};

inline dword getInterruptGateTargetSegmentOffset(const _tagInterruptTrapGateDescriptor* pigd)
{
    return pigd->codsegoffset15 | (pigd->codsegoffset31 << 16);
}

inline dword getTrapGateTargetSegmentOffset(const _tagInterruptTrapGateDescriptor* pigd)
{
    return pigd->codsegoffset15 | (pigd->codsegoffset31 << 16);
}

struct _tagTaskGateDescriptor
{
    IGN_X(dword, 16);
    BIT_FIELDS(dword, codsegselector, 16); // TSS selector
    IGN_X(dword, Type, 5);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    IGN_X(dword, 16);
};

struct _tagCodeSegmentDescriptorL
{
    IGN_W(dword);
    IGN_X(dword, 10);
    BIT_FIELD(dword, C);
    IGN_X(dword, 2);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    IGN_X(dword, 5);
    BIT_FIELD(dword, L); // 1 - 64bit mode, 0 - compatibility mode
    BIT_FIELD(dword, D);
    IGN_X(dword, 9);
};

struct _tagDataSegmentDescriptorL
{
    IGN_W(dword);
    IGN_X(dword, 15);
    BIT_FIELD(dword, P);
    IGN_X(dword, 16);
};

#define DT_TypeL_Sys_LDT64            0x2u
#define DT_TypeL_Sys_TSS64A           0x9u
#define DT_TypeL_Sys_TSS64B           0xBu
#define DT_TypeL_Sys_CALLGATE64       0xCu
#define DT_TypeL_Sys_TNTERRUPTGATE64  0xEu
#define DT_TypeL_Sys_TRAPGATE         0xFu

struct _tagSystemSegmentDescriptorl
{
    BIT_FIELDS(dword, seglimit15, 16); // segment limit 0-15
    BIT_FIELDS(dword, segbaseaddr15, 16); // base address 0-15
    BIT_FIELDS(dword, segbaseaddr23, 8); // base address 16-23
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2); // descriptor privilege level
    // present, indicates whether the segment referenced by the descriptor 
    // is loaded in memory, OS set, processor Never modify, if 0 #NP
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, seglimit19, 4); // segment limit 16-19
    BIT_FIELDS(dword, AVL); // avilable to software, processor Never modify
    RESR_(dword, 2);
    BIT_FIELD(dword, G); // gradularity, 1 means 4kb
    BIT_FIELDS(dword, segbaseaddr31, 8); // base address 24-31
    BIT_FIELDSW(dword, segbaseaddr63); // base address 32-63
    IGN_X(dword, 8);
    RESR_MBZ(dword, 5);
    IGN_X(dword, 19);
};

struct _tagCallGateDescriptorl
{
    BIT_FIELDS(dword, tgtoffset15, 16); // target offset 0-15
    BIT_FIELDS(dword, tgtselector, 16); // target selector
    IGN_X(dword, 8);
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, tgtoffset31, 4); // target offset 16-31
    BIT_FIELDSW(dword, tgtoffset63); // target offset 32-63
    IGN_X(dword, 8);
    RESR_MBZ(dword, 5);
    IGN_X(dword, 19);
};

inline qword getCallGateTargetSegmentOffset(const _tagCallGateDescriptorl* pcgdl)
{
    qword _ret = pcgdl->tgtoffset63;
    _ret <<= 32;
    return _ret | (pcgdl->tgtoffset15 | (pigd->tgtoffset31 << 16));
}

struct _tagInterruptTrapGateGateDescriptorl
{
    BIT_FIELDS(dword, tgtoffset15, 16); // target offset 0-15
    BIT_FIELDS(dword, tgtselector, 16); // target selector
    BIT_FIELDS(dword, IST, 2);
    IGN_X(dword, 6);
    BIT_FIELDS(dword, Type, 4);
    BIT_FIELD(dword, S);
    BIT_FIELDS(dword, DPL, 2);
    BIT_FIELD(dword, P); 
    BIT_FIELDS(dword, tgtoffset31, 4); // target offset 16-31
    BIT_FIELDSW(dword, tgtoffset63); // target offset 32-63
    IGN_W(dword);
};

inline qword getInterruptGateTargetSegmentOffset(const _tagInterruptTrapGateDescriptorl* pcgdl)
{
    qword _ret = pcgdl->tgtoffset63;
    _ret <<= 32;
    return _ret | (pcgdl->tgtoffset15 | (pigd->tgtoffset31 << 16));
}

inline qword getTrapGateTargetSegmentOffset(const _tagInterruptTrapGateDescriptorl* pcgdl)
{
    qword _ret = pcgdl->tgtoffset63;
    _ret <<= 32;
    return _ret | (pcgdl->tgtoffset15 | (pigd->tgtoffset31 << 16));
}

typedef struct _tagVirtualAddress4K
{
    BIT_FIELDS(dword, PO, 12); // page offset
    BIT_FIELDS(dword, PTO, 10); // page table offset
    BIT_FIELDS(dword, PDO, 10); // page directory offset
};

/* Filed Name Specified
P      -- Present
RorW   -- Read or Write
UorS   -- User or Supervisor
PWT    -- Page-level Write Though
PCD    -- Page-level Cache Disabled
A      -- Accessed
D      -- Dirty
PAT    -- PAT
PS     -- Page Size
G      -- Global
XD     -- eXecute Disable
*/

typedef struct _tagPageDirectoryEntry4K
{
    BIT_FIELD(dword, P); //
    BIT_FIELD(dword, RorW); //
    BIT_FIELD(dword, UorS); //
    BIT_FIELD(dword, PWT); //
    BIT_FIELD(dword, PCD); //
    BIT_FIELD(dword, A); //
    IGN_(dword); //
    RESR_MBZ(dword); //
    IGN_(dword); //
    BIT_FIELDS(dword, AVL, 3); //
    BIT_FIELDS(dword, PTE, 20); // page table base address
};

typedef struct _tagPageTableEntry4K
{
    BIT_FIELD(dword, P); //
    BIT_FIELD(dword, RorW); //
    BIT_FIELD(dword, UorS); //
    BIT_FIELD(dword, PWT); //
    BIT_FIELD(dword, PCD); //
    BIT_FIELD(dword, A); //
    BIT_FIELD(dword, D); //
    BIT_FIELD(dword, PAT); //
    BIT_FIELD(dword, G); //
    BIT_FIELDS(dword, AVL, 3); //
    BIT_FIELDS(dword, PPE, 20); // physical page base address
};

typedef struct _tagVirtualAddress4M
{
    BIT_FIELDS(dword, PO, 22); // page offset
    BIT_FIELDS(dword, PDO, 10); // page directory offset
};


typedef struct _tagPageDirectoryEntry4M
{
    BIT_FIELD(dword, P); //
    BIT_FIELD(dword, RorW); //
    BIT_FIELD(dword, UorS); //
    BIT_FIELD(dword, PWT); //
    BIT_FIELD(dword, PCD); //
    BIT_FIELD(dword, A); //
    BIT_FIELD(dword, D)
    RESR_MBO(dword); //
    BIT_FIELD(dword, G)
    BIT_FIELDS(dword, AVL, 3); //
    BIT_FIELD(dword, PAT); //
    BIT_FIELDS(dword, PPE39, 8) // physical page base address 32-39
    RESR_MBZ(dword); //
    BIT_FIELDS(dword, PPE31, 10); // physical page base address 22-31
};

typedef struct _tagVirtualAddress4KA
{
    BIT_FIELDS(dword, PO, 12); // page offset
    BIT_FIELDS(dword, PTO, 9); // page table offset
    BIT_FIELDS(dword, PDO, 9); // page directory offset
    BIT_FIELDS(dword, PDPO, 2); // page directory pointer offset
};

typedef struct _tagPageDirectoryPointerEntry4KA
{
    BIT_FIELD(qword, P); //
    RESR_MBZX(qword, 2); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    RESR_MBZX(qword, 4); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDE, 40); // page directory base address
    RESR_MBZX(qword, 12); //
};

typedef struct _tagPageDirectoryEntry4KA
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    IGN_(qword);
    RESR_MBZ(qword); //
    IGN_(qword);
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PTE, 40); // page table base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageTableEntry4KA
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    BIT_FIELD(qword, D);
    BIT_FIELD(qword, PAT); //
    BIT_FIELD(qword, G);
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PPE, 40); // physical page base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagVirtualAddress2MA
{
    BIT_FIELDS(dword, PO, 21); // page offset
    BIT_FIELDS(dword, PDO, 9); // page directory offset
    BIT_FIELDS(dword, PDPO, 2); // page directory pointer offset
};

typedef struct _tagPageDirectoryPointerEntry2MA
{
    BIT_FIELD(qword, P); //
    RESR_MBZX(qword, 2); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    RESR_MBZX(qword, 4); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDE, 40); // page directory base address
    RESR_MBZX(qword, 12); //
};

typedef struct _tagPageDirectoryEntry2MA
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    BIT_FIELD(qword, D); //
    RESR_MBO(qword); //
    BIT_FIELD(qword, G); //
    BIT_FIELDS(qword, AVL, 3); //
    BIT_FIELD(qword, PAT); //
    RESR_MBZX(qword, 18); //
    __archi_limit BIT_FIELD(qword, PPE, 31); // physcal page base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagVirtualAddress4KL
{
    BIT_FIELDS(qword, PO, 12); // page offset
    BIT_FIELDS(qword, PTO, 9); // page table offset
    BIT_FIELDS(qword, PDO, 9); // page directory offset
    BIT_FIELDS(qword, PDPO, 9); // page directory pointer offset
    BIT_FIELDS(qword, PML4O, 9); // page map level 4 offset
    RESR_X(qword, 16); // sign extended bit 47
};

typedef struct _tagPageMapLevel4Entry4KL
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    IGN_(qword);
    RESR_MBZX(qword, 2); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDPE, 40); // page directory pointer base address
    IGN_X(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageDirectoryPointerEntry4KL
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    IGN_(qword);
    RESR_MBZX(qword, 2); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDE, 40); // page directory base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageDirectoryEntry4KL
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    IGN_(qword);
    RESR_MBZ(qword); //
    IGN_(qword);
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PTE, 40); // page table base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageTableEntry4KL
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    BIT_FIELD(qword, D); //
    BIT_FIELD(qword, PAT); //
    BIT_FIELD(qword, G); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PPE, 40); // physical page base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagVirtualAddress2ML
{
    BIT_FIELDS(qword, PO, 21); // page offset
    BIT_FIELDS(qword, PDO, 9); // page directory offset
    BIT_FIELDS(qword, PDPO, 9); // page directory pointer offset
    BIT_FIELDS(qword, PML4O, 9); // page map level 4 offset
    RESR_X(qword, 16); // sign extended bit 47
};

typedef struct _tagPageMapLevel4Entry2ML
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    RESR_MBZX(qword, 3); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDPE, 40); // page directory pointer base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageDirectoryPointerEntry2ML
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    RESR_MBZX(qword, 3); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDE, 40); // page directory base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageDirectoryEntry2ML
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    BIT_FIELD(qword, D); //
    RESR_MBO(qword); 
    BIT_FIELD(qword, G);
    BIT_FIELDS(qword, AVL 3); //
    BIT_FIELD(qword, PAT); //
    RESR_MBZ(qword, 8);
    __archi_limit BIT_FIELD(qword, PPE, 31); // physical page base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagVirtualAddress1GL
{
    BIT_FIELDS(qword, PO, 30); // page offset
    BIT_FIELDS(qword, PDO, 9); // page directory offset
    BIT_FIELDS(qword, PML4O, 9); // page map level 4 offset
    RESR_X(qword, 16); // sign extended bit 47
};

typedef struct _tagPageMapLevel4Entry1GL
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    RESR_MBZX(qword, 3); //
    BIT_FIELDS(qword, AVL 3); //
    __archi_limit BIT_FIELD(qword, PDPE, 40); // page directory pointer base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};

typedef struct _tagPageDirectoryPointerEntry1GL
{
    BIT_FIELD(qword, P); //
    BIT_FIELD(qword, RorW); //
    BIT_FIELD(qword, UorS); //
    BIT_FIELD(qword, PWT); //
    BIT_FIELD(qword, PCD); //
    BIT_FIELD(qword, A); //
    BIT_FIELD(qword, D); //
    RESR_MBO(qword); //
    BIT_FIELD(qword, G); //
    BIT_FIELDS(qword, AVL 3); //
    BIT_FIELD(qword, PAT); //
    RESR_MBZX(qword, 17); //
    __archi_limit BIT_FIELD(qword, PPE, 22); // physical page base address
    RESR_MBZX(qword, 11); //
    BIT_FIELD(qword, NX);
};
#endif

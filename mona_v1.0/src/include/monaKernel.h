/*!
    \file  monaKernel.h
    \brief definition for kernel

    definition for kernel & macros
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#ifndef _MONA_KERNEL_
#define _MONA_KERNEL_

#include <monaTypes.h>

#define _sysLock()   asm("cli") /*! disable interupts */
#define _sysUnlock() asm("sti") /*! enable  interupts */

/*! TSS */
typedef struct {
    word  backlink;
    word  pad0;
    dword esp0;
    word  ss0;
    word  pad1;
    dword esp1;
    word  ss1;
    word  pad2;
    dword esp2;
    word  ss2;
    word  pad3;
    dword cr3;
    dword eip;
    dword eflags;
    dword eax;
    dword ecx;
    dword edx;
    dword ebx;
    dword esp;
    dword ebp;
    dword esi;
    dword edi;
    word  es;
    word  pad4;
    word  cs;
    word  pad5;
    word  ss;
    word  pad6;
    word  ds;
    word  pad7;
    word  fs;
    word  pad8;
    word  gs;
    word  pad9;
    word  ldt;
    word  padA;
    word  debugtrap;
    word  iobase;
} TSS;

/*! gdtr */
typedef struct {
    word  limit;
    word  dummy;
    word  dummy2;
    dword base;
} GDTR;

/*! gdt */
typedef struct {
    word limitL;
    word baseL;
    byte baseM;
    byte type;
    byte limitH;
    byte baseH;
} GDT;

/*! Descriptor Type */
#define TypeCode     0x9a
#define TypeData     0x92
#define TypeStack    0x96
#define TypeLDT      0x82
#define TypeTSS      0x89
#define TypeTSSBusy  0x8b
#define TypeCallGate 0x84
#define TypeIntrGate 0x8e
#define TypeTrapGate 0x8f
#define TypeTaskGate 0x85

void startKernel(void);

#endif

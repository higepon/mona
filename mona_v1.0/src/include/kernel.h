/*!
    \file  kernel.h
    \brief definition for kernel

    definition for kernel & macros
    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#ifndef _MONA_KERNEL_
#define _MONA_KERNEL_

#include <types.h>
#include <VirtualConsole.h>

#define disableInterrupt() asm volatile("cli")      /*!< \def disable interupts */
#define enableInterrupt()  asm volatile("sti")      /*!< \def enable  interupts */

typedef struct {

    dword stack0;
    dword stack1;
    dword stack2;
    dword stack3;
    dword stack4;
    dword stack5;
    dword stack6;
    dword stack7;
} StackView;

/*!
   \struct FARJMP
*/
typedef struct {
    dword offset;
    word selector;
} FARJMP;

/*!
    \struct TSS
*/
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

void startKernel(void);
void panic(const char*);
inline void printOK(const char*);
inline void printBanner();
#endif

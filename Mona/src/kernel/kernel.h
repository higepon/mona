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

#include <sys/types.h>
#include "VirtualConsole.h"

extern "C" void rectangle(int x, int y, int xx, int yy, char color);

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

typedef struct {
    dword eax;   //0
    dword ebx;
    dword ecx;   //2
    dword edx;
    dword esp;   //4
    dword ebp;
    dword esi;   //6
    dword edi;
    dword cs;    //8
    dword ds;
    dword ss;    //10
    dword cr3;
    dword eflags;//12
} DokodemoView;

/*!
    \struct TSS
*/
typedef struct {
    word  backlink;//0
    word  pad0;
    dword esp0;//1
    word  ss0;//2
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

struct ThreadInfo;

typedef struct
{
    bool hasUserHandler;
    void* handler;
    ThreadInfo* thread;
} IRQInfo;

void startKernel(void);
void panic(const char*);
void checkMemoryAllocate(void* p, const char* msg);
inline void printOK(const char*);
inline void printBanner();
void rdtscsub(dword* timeL, dword* timeH);
void rdtsc(dword* timeL, dword* timeH);

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define onAssertError() panic("assert");
#define ASSERT(condition) {if (!(condition)) {g_console->printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);onAssertError();}}
#else
#define ASSERT(condition) {}
#endif

#endif

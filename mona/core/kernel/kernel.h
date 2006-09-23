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


#define FIRST_THREAD_ID (56)

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
    volatile    word  backlink;     //0
    volatile    word  pad0;
    volatile    dword esp0;         //1
    volatile    word  ss0;          //2
    volatile    word  pad1;
    volatile    dword esp1;
    volatile    word  ss1;
    volatile    word  pad2;
    volatile    dword esp2;
    volatile    word  ss2;
    volatile    word  pad3;
    volatile    dword cr3;
    volatile    dword eip;
    volatile    dword eflags;
    volatile    dword eax;
    volatile    dword ecx;
    volatile    dword edx;
    volatile    dword ebx;
    volatile    dword esp;
    volatile    dword ebp;
    volatile    dword esi;
    volatile    dword edi;
    volatile    word  es;
    volatile    word  pad4;
    volatile    word  cs;
    volatile    word  pad5;
    volatile    word  ss;
    volatile    word  pad6;
    volatile    word  ds;
    volatile    word  pad7;
    volatile    word  fs;
    volatile    word  pad8;
    volatile    word  gs;
    volatile    word  pad9;
    volatile    word  ldt;
    volatile    word  padA;
    volatile    word  debugtrap;
    volatile    word  iobase;
} TSS;

struct ThreadInfo;

typedef struct
{
    ThreadInfo* thread;
    bool hasReceiver;
    bool maskInterrupt;
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

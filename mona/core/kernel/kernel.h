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

    uint32_t stack0;
    uint32_t stack1;
    uint32_t stack2;
    uint32_t stack3;
    uint32_t stack4;
    uint32_t stack5;
    uint32_t stack6;
    uint32_t stack7;
} StackView;

typedef struct {
    uint32_t eax;   //0
    uint32_t ebx;
    uint32_t ecx;   //2
    uint32_t edx;
    uint32_t esp;   //4
    uint32_t ebp;
    uint32_t esi;   //6
    uint32_t edi;
    uint32_t cs;    //8
    uint32_t ds;
    uint32_t ss;    //10
    uint32_t cr3;
    uint32_t eflags;//12
} DokodemoView;

/*!
    \struct TSS
*/
typedef struct {
    volatile    uint16_t  backlink;     //0
    volatile    uint16_t  pad0;
    volatile    uint32_t esp0;         //1
    volatile    uint16_t  ss0;          //2
    volatile    uint16_t  pad1;
    volatile    uint32_t esp1;
    volatile    uint16_t  ss1;
    volatile    uint16_t  pad2;
    volatile    uint32_t esp2;
    volatile    uint16_t  ss2;
    volatile    uint16_t  pad3;
    volatile    uint32_t cr3;
    volatile    uint32_t eip;
    volatile    uint32_t eflags;
    volatile    uint32_t eax;
    volatile    uint32_t ecx;
    volatile    uint32_t edx;
    volatile    uint32_t ebx;
    volatile    uint32_t esp;
    volatile    uint32_t ebp;
    volatile    uint32_t esi;
    volatile    uint32_t edi;
    volatile    uint16_t  es;
    volatile    uint16_t  pad4;
    volatile    uint16_t  cs;
    volatile    uint16_t  pad5;
    volatile    uint16_t  ss;
    volatile    uint16_t  pad6;
    volatile    uint16_t  ds;
    volatile    uint16_t  pad7;
    volatile    uint16_t  fs;
    volatile    uint16_t  pad8;
    volatile    uint16_t  gs;
    volatile    uint16_t  pad9;
    volatile    uint16_t  ldt;
    volatile    uint16_t  padA;
    volatile    uint16_t  debugtrap;
    volatile    uint16_t  iobase;
} TSS;

struct ThreadInfo;

typedef struct
{
    ThreadInfo* thread;
    bool hasReceiver;
    bool maskInterrupt;
} IRQInfo;

void startKernel(void);
extern "C" void panic(const char*);
void checkMemoryAllocate(void* p, const char* msg);
inline void printOK(const char*);
inline void printBanner();
void rdtscsub(uint32_t* timeL, uint32_t* timeH);
void rdtsc(uint32_t* timeL, uint32_t* timeH);

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define onAssertError() panic("assert");
#define ASSERT(condition) {if (!(condition)) {g_console->printf("ASSERT failure %s:%d: %s\n", __FILE__, __LINE__, #condition);onAssertError();}}
#else
#define ASSERT(condition) {}
#endif

#endif

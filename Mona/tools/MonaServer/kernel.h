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

#ifndef __KERNEL_H__
#define __KERNEL_H__

extern "C" void rectangle(int x, int y, int xx, int yy, char color);

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

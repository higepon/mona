/*!
    \file  kthread.h
    \brief kernel thread

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#ifndef _MONA_KTHREAD_
#define _MONA_KTHREAD_

#include<monaTypes.h>

/*! struct for kernel thread */
typedef struct Kthread {
    dword  eip;
    dword  cs;
    dword  eflags;
    dword  eax;
    dword  ecx;
    dword  edx;
    dword  ebx;
    dword  esp;
    dword  ebp;
    dword  esi;
    dword  edi;
    dword  tick;
};

extern Kthread* current;


void kthread_init();
void kthread_tick();


#endif

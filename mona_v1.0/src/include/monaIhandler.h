/*!
    \file  monaIhandler.h
    \brief interrupt handlers

    definition for IDT
    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/24 update:$Date$
*/
#ifndef _MONA_IHANDLER_
#define _MONA_IHANDLER_

#include <kthread.h>

extern "C" void arch_fdchandler(void);
extern "C" void arch_timerhandler(void);
extern "C" void arch_keystrokehandler(void);
extern "C" void arch_dummyhandler(void);
extern "C" void arch_kthread_switch(void);

extern "C" void MFDCHandler(void);
extern "C" void timerHandler(void);
extern "C" void keyStrokeHandler(void);
extern "C" void dummyHandler(void);

extern "C" dword eip;
extern "C" dword cs;
extern "C" dword eflags;
extern "C" Kthread* current;

#endif

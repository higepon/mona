/*!
    \file  io.h
    \brief io functions and difinitions

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/03/08 update:$Date$
*/

#ifndef _MONA_IO_
#define _MONA_IO_

#include <sys/types.h>
#include "global.h"

extern "C" uint8_t inp8(uint32_t port);
extern "C" void outp8(uint32_t port, uint8_t value);
extern "C" uint16_t inp16(uint32_t port);
extern "C" void outp16(uint32_t port, uint16_t value);
extern "C" uint32_t inp32(uint32_t port);
extern "C" void outp32(uint32_t port, uint32_t value);
extern "C" uint32_t get_eflags();
extern "C" void  set_eflags(uint32_t eflags);

extern VirtualConsole*g_console;

#endif

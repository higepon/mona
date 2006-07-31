/*!
    \file  io.h
    \brief io functions and difinitions

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/08 update:$Date$
*/

#ifndef _MONA_IO_
#define _MONA_IO_

#include <sys/types.h>
#include "global.h"

byte inp8(dword port);
void outp8(dword port, byte value);
word inp16(dword port);
void outp16(dword port, word value);
dword inp32(dword port);
void outp32(dword port, dword value);
dword get_eflags();
void  set_eflags(dword eflags);

extern VirtualConsole*g_console;

#endif

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

#include <types.h>
#include <global.h>

byte inportb(dword);
void outportb(dword, byte);
dword get_eflags();
void  set_eflags(dword eflags);

extern VirtualConsole*g_console;
class IOManager {

  public:
    inline static void grantPermission() {

        g_console->printf("eflags=%x", get_eflags());
        dword eflags = get_eflags();
        set_eflags(eflags | IOPL3);
        g_console->printf("eflags=%x", get_eflags());
    }

    static const dword IOPL3 = 0x3000;
};

#endif

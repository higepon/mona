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

#include <monaTypes.h>

byte inportb(dword);
void outportb(dword, byte);
dword get_eflags();
void  set_eflags(dword eflags);
#endif

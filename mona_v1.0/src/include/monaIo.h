/*!
    \file  monaIo.h
    \brief io functions and difinitions

    io functions and difinitions
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/29 update:$Date$
*/

#ifndef _MONA_IO_
#define _MONA_IO_

#include <monaTypes.h>

unsigned char inportb(dword);
void outportb(dword, byte);
void _sysInitIo();

#endif

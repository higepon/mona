/*!
    \file  higeIo.h
    \brief io functions and difinitions

    io functions and difinitions
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/29 update:$Date$
*/

#ifndef _HIGEPOS_IO_
#define _HIGEPOS_IO_

unsigned char inportb(unsigned int);
void outportb(unsigned int,unsigned char);
void _sysInitIo();

#endif

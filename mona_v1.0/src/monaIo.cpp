/*!
    \file   monaIo.cpp
    \brief  io functions

    io functions

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/29 update:$Date$
*/
#include <monaIo.h>

/*!
    \brief initilize I/O

    initilize I/O,PIC

    \author HigePon
    \date   create:2002/08/02 update:2002/10/23
*/
void _sysInitIo() {

    /* Enable IRQ0 (timer) and IRQ1 (keyboard) at the
    8259 PIC chips, disable others: */
    outportb(0x21, ~0x43);
    outportb(0xA1, ~0x00);
    return;
}

/*!
    \brief input

    input

    \param  port port number

    \author HigePon
    \date   create:2002/07/29 update:2002/12/26
*/
unsigned char inportb(dword port) {

    unsigned char ret;
    asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
    return ret;
}

/*!
    \brief output

    output

    \param  port  port number
    \param  value output value
    \author HigePon
    \date   create:2002/07/29 update:
*/
void outportb(dword port, byte value) {
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

/*!
    \file   io.cpp
    \brief  io functions

    io functions

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/29 update:$Date$
*/
#include <io.h>

void _sysInitIo() {

    /* Enable IRQ0 (timer) and IRQ1 (keyboard) at the
    8259 PIC chips, disable others: */
    outportb(0x21, ~0x03);
    outportb(0xA1, ~0x00);
    return;
}

/*!
    \brief input

    input

    \param  port port number

    \author HigePon
    \date   create:2002/07/29 update:
*/
unsigned char inportb(unsigned int port) {

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
void outportb(unsigned int port,unsigned char value) {
   asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));
}

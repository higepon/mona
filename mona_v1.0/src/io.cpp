/*!
    \file  io.cpp
    \brief io functions

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include<io.h>

/*!
    \brief input

    \param  port port number

    \author HigePon
    \date   create:2002/07/29 update:2002/12/26
*/
byte inportb(dword port) {

    byte ret;
    asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
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
   asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
}

/*!
    \brief get eflags

    \return eflags
    \author HigePon
    \date   create:2002/03/21 update:
*/
dword get_eflags() {

    dword result;

    asm volatile("pushfl           \n"
                 "pop    %%eax     \n"
                 "movl   %%eax, %0 \n"
                 : "=m"(result) : /* no input */ : "eax");
    return result;
}

/*!
    \brief set eflags

    \param eflags eflags to set
    \author HigePon
    \date   create:2002/03/21 update:
*/
void  set_eflags(dword eflags) {

    asm volatile("movl   %0, %%eax \n"
                 "push   %%eax     \n"
                 "popfl            \n"
                 : /* no output */ : "m"(eflags) : "eax");

    return;
}

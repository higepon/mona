/*!
    \file  io.cpp
    \brief io functions

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include "io.h"

byte inp8(dword port) {

    byte ret;
    asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
    return ret;
}

void outp8(dword port, byte value) {
   asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
}

word inp16(dword port) {

    word ret;
    asm volatile ("inw %%dx, %%ax": "=a"(ret): "d"(port));
    return ret;
}

void outp16(dword port, word value) {
   asm volatile ("outw %%ax, %%dx": :"d" (port), "a" (value));
}

dword inp32(dword port) {

    dword ret;
    asm volatile ("inl %%dx, %%eax": "=a"(ret): "d"(port));
    return ret;
}

void outp32(dword port, dword value) {
   asm volatile ("outl %%eax, %%dx": :"d" (port), "a" (value));
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

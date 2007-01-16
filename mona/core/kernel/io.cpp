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

uint8_t inp8(uint32_t port) {

    uint8_t ret;
    asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
    return ret;
}

void outp8(uint32_t port, uint8_t value) {
   asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
}

uint16_t inp16(uint32_t port) {

    uint16_t ret;
    asm volatile ("inw %%dx, %%ax": "=a"(ret): "d"(port));
    return ret;
}

void outp16(uint32_t port, uint16_t value) {
   asm volatile ("outw %%ax, %%dx": :"d" (port), "a" (value));
}

uint32_t inp32(uint32_t port) {

    uint32_t ret;
    asm volatile ("inl %%dx, %%eax": "=a"(ret): "d"(port));
    return ret;
}

void outp32(uint32_t port, uint32_t value) {
   asm volatile ("outl %%eax, %%dx": :"d" (port), "a" (value));
}

/*!
    \brief get eflags

    \return eflags
    \author HigePon
    \date   create:2002/03/21 update:
*/
uint32_t get_eflags() {

    uint32_t result;

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
void  set_eflags(uint32_t eflags) {

    asm volatile("movl   %0, %%eax \n"
                 "push   %%eax     \n"
                 "popfl            \n"
                 : /* no output */ : "m"(eflags) : "eax");

    return;
}

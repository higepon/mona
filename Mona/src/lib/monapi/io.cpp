/*! \file
    \brief monapi io functions

    \author Higepon
    \date   create:2004/10/30 update:$Date$
    $Revision$
*/
#include <monapi/io.h>

/*!
    control irq interrupt.

    \param irq      [in] irq number 0-15
    \param enabled  [in] enable interrupt?
    \param auto_ir2 [in] if 8 <= irq <= 15 and enables = true, auto_ir2 indicate whether enable ir2.
    \return none
*/
void monapi_set_irq(int irq, MONAPI_BOOL enabled, MONAPI_BOOL auto_ir2)
{
    if (irq >= 0 && irq <= 7)
    {
        if (enabled == MONAPI_TRUE) outp8(0x21, (inp8(0x21) & ~(1 << irq)));
        else outp8(0x21, (inp8(0x21) | (1 << irq)));
    }
    else if (irq >= 8 && irq <= 15)
    {
        if (enabled == MONAPI_TRUE) outp8(0xa1, inp8(0xa1) & ~(1 << (irq - 8)));
        else outp8(0xa1, inp8(0xa1) | (1 << (irq - 8)));

        if (enabled == MONAPI_TRUE && auto_ir2 == MONAPI_TRUE)
        {
            outp8(0x21, (inp8(0x21) & ~(1 << 2)));
        }
    }
}

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

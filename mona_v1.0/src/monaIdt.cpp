/*!
    \file   monaIdt.cpp
    \brief  set up idt

    set up idt for interrupts

    Copyright (c) 2002,2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/23 update:$Date$
*/
#include <monaIdt.h>
#include <monaIo.h>

/*!
    \brief set up idt

    set up idt.

    memo
    short 2byte
    int   4byte

    \author HigePon
    \date   create:2002/07/23 update:
*/
void _sysSetIdt() {

    /* idtr */
    struct idtr_st idtr;

    /* handler list */
    extern handler_st handlers[HANDLER_NUM];

    /* set all idt */
    for (int i = 0; i < HANDLER_NUM; i++) {
        setIdt(&(handlers[i]), 0x08);
    }

    /* idtr set up */
    idtr.limit    = sizeof(idt_st) * HANDLER_NUM -1;
    idtr.highbase = IDT_HIGHBASE;
    idtr.lowbase  = IDT_LOWBASE;

    _sysLoadIdtr(&idtr);

    return;
}

/*!
    \brief set up one idt

    set up one idt.

    \param  p        pointer to handler_st
    \param  selector segment selector

    \author HigePon
    \date   create:2002/07/25 update:2002/07/27
*/
void setIdt(handler_st* p, int selector) {

    /* idt size = 64bit */
    idt_st* idt = ((idt_st*) IDT_BASE) + p->number;

    /* set idt */
    idt->offsetL  = (int)(p->handler) & 0x0000FFFF;
    idt->offsetH  = ((int)(p->handler) & 0xFFFF0000) >> 16;
    idt->selector = selector;
    idt->type     = 0x8E;
    idt->unused   = IDT_UNUSED;

    return;
}

/*!
    \brief lidt

    lidt

    \author HigePon
    \date   create:2002/07/25 update:
*/
void _sysLoadIdtr(idtr_st* idtr) {

    asm volatile("lidt (%0) ": :"p" (idtr));
    return;
}

/*!
    \brief disable timer

    disable timer interrupt
    mask IMR bit 0

    \author HigePon
    \date   create:2003/01/12 update:
*/
void disableTimer() {

    outportb(0x21, inportb(0x21) | 0x01);
}

/*!
    \brief enable timer

    enable timer interrupt

    \author HigePon
    \date   create:2003/01/12 update:
*/
void enableTimer() {

    outportb(0x21, inportb(0x21) & 0xFE);
}

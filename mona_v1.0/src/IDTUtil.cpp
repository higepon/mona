/*!
    \file  IDTUtil.cpp
    \brief class functions for set up IDT

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/08 update:$Date$
*/

#include<global.h>
#include<operator.h>
#include<IDTUtil.h>

/*!
    \brief call lidt

    \param idtr idtr
    \author HigePon
    \date   create:2003/06/08 update:
*/
void IDTUtil::lidt(IDTR* idtr) {

    asm volatile("lidt (%0) ": :"p" (idtr));
    return;
}

/*!
    \brief set gate descriptor

    \param descZero pointer to idt
    \param selector selector
    \param handler handler

    \author HigePon
    \date   create:2003/06/08 update:
*/
void IDTUtil::setGateDesc(GateDesc* descZero, word selector, InterruptHandlers* handler) {

    GateDesc* desc = descZero + handler->number;

    desc->offsetL  = (dword)(handler->handler) & 0x0000FFFF;
    desc->offsetH  = ((dword)(handler->handler) & 0xFFFF0000) >> 16;
    desc->selector = selector;
    desc->type     = 0x8E;
    desc->unused   = 0x00;
    return;
}

/*!
    \brief set up IDT

    \author HigePon
    \date   create:2003/06/08 update:
*/
void IDTUtil::setup() {

    g_idt = (GateDesc*)malloc(sizeof(GateDesc) * IHANDLER_NUM);

    extern InterruptHandlers handlers[IHANDLER_NUM];

    for (int i = 0; i < IHANDLER_NUM; i++) {

        setGateDesc(g_idt, 0x08, &handlers[i]);
    }

    /* lidt */
    IDTR idtr;
    idtr.base  = (dword)g_idt;
    idtr.limit = sizeof(GateDesc) * IHANDLER_NUM - 1;
    lidt(&idtr);
    return;
}


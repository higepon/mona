/*!
    \file  GDTUtil.cpp
    \brief class functions for set up GDT

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2003/06/06 update:$Date$
*/

#include "global.h"
#include "operator.h"
#include "kernel.h"
#include "GDTUtil.h"

/*!
    \brief set segment descriptor

    \param  desc  segment descriptor
    \param  base  base address 32bit
    \param  limit limit        24bit include G, D/B, 0, AVL
    \param  type  segment type 8bit  include P, DPL, S, TYPE
    \author Higepon
    \date   create:2003/06/06 update:
*/
void GDTUtil::setSegDesc(SegDesc* desc, uint32_t base, uint32_t limit, uint8_t type) {

    desc->baseL  = (uint16_t)(base & 0xFFFF);
    desc->baseM  = (uint8_t)((base >> 16) & 0xFF);
    desc->baseH  = (uint8_t)((base >> 24) & 0xFF);
    desc->limitL = (uint16_t)(limit & 0xFFFF);
    desc->limitH = (uint8_t) (((limit >> 16) & 0x0F) | 0xC0); /* 4KB unit & 32bit segment */
    desc->type   = type;
    return;
}

/*!
    \brief set segment descriptor

    \param  desc   segment descriptor
    \param  base   base address 32bit
    \param  limit  limit        24bit
    \param  type   segment type 8bit  include P, DPL, S, TYPE
    \param  gdbavl some flags.
    \author Yume
    \date   create:2006/08/01 update:
*/
void GDTUtil::setSegDescExt(SegDesc* desc, uint32_t base, uint32_t limit, uint8_t type, uint8_t gdbavl) {

    desc->baseL  = (uint16_t)(base & 0xFFFF);
    desc->baseM  = (uint8_t)((base >> 16) & 0xFF);
    desc->baseH  = (uint8_t)((base >> 24) & 0xFF);
    desc->limitL = (uint16_t)(limit & 0xFFFF);
    desc->limitH = (uint8_t) (((limit >> 16) & 0x0F) | gdbavl);
    desc->type   = type;
    return;
}

/*!
    \brief call lgdt

    \param gdtr gdtr
    \author Higepon
    \date   create:2003/06/07 update:2003/06/08
*/
void GDTUtil::lgdt(GDTR* gdtr) {

    asm volatile("lgdt %0\n" : /* no output */ : "m" (*gdtr));
    return;
}

/*!
    \brief do ltr

    do ltr, load selector value into tr register

    \param selector selector value

    \author Higepon
    \date   create:2002/12/02 update:
*/
void GDTUtil::ltr(uint16_t selector) {

    /* ltr */
    asm volatile("ltr %0\n": "=m" (selector));
    return;
}

/*!
    \brief set up GDT

    \author Higepon
    \date   create:2003/06/07 update:2003/07/18
*/
void GDTUtil::setup() {

    g_gdt = (SegDesc*)malloc(sizeof(SegDesc) * GDT_ENTRY_NUM);
    checkMemoryAllocate(g_gdt, "GDT Memory allocate");

    /* NULL */
    setSegDesc(&g_gdt[0], 0, 0, 0);
    g_gdt[0].limitH = 0;

    /* allcate TSS */
    g_tss = (TSS*)malloc(sizeof(TSS));

    /* SYS CS 0-4GB */
    setSegDesc(&g_gdt[1], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A);

    /* SYS DS 0-4GB */
    setSegDesc(&g_gdt[2], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x02);

    /* SYS SS 0-4GB */
    setSegDesc(&g_gdt[3], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x02);

    /* TSS. Mona has only one TSS */
    setSegDesc(&g_gdt[4], (uint32_t)g_tss, 0x00000067, SEGMENT_PRESENT | SEGMENT_DPL0 | 0x00 | 0x09);

    /* USER CS 0-4GB */
    setSegDesc(&g_gdt[5], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL3 | 0x10 | 0x0A);

    /* USER DS 0-4GB */
    setSegDesc(&g_gdt[6], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL3 | 0x10 | 0x02);

    /* USER SS 0-4GB */
    setSegDesc(&g_gdt[7], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL3 | 0x10 | 0x02);

    if( g_apmInfo->isSupported )
    {
        /* APM 32bit CS */
        setSegDescExt(&g_gdt[8], g_apmInfo->cs32<<4, (g_apmInfo->cs32_len-1),
                            SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A, 0xC0);
        /* APM 16bit CS */
        setSegDescExt(&g_gdt[9], g_apmInfo->cs16<<4, (g_apmInfo->cs16_len-1),
                            SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A, 0x80);
        /* APM DS */
        setSegDescExt(&g_gdt[10], g_apmInfo->ds<<4, (g_apmInfo->cs32_len-1),
                            SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x02, 0xC0);
    }


    /* lgdt */
    GDTR gdtr;
    gdtr.base  = (uint32_t)g_gdt;
    gdtr.limit = sizeof(SegDesc) * GDT_ENTRY_NUM - 1;
    lgdt(&gdtr);

    /* setup TSS */
    setupTSS(0x20);

    return;
}

/*!
    \brief set up TSS

    \author Higepon
    \date   create:2003/07/17 update:2003/08/06
*/
void GDTUtil::setupTSS(uint16_t selector) {

    /* prepare dpl0 stack */
    memset((void*)g_tss, 0, sizeof(TSS));
    g_tss->esp0 = 0x90000;
    g_tss->ss0  = KERNEL_SS;

    /* load TSS */
    ltr(selector);

    return;
}

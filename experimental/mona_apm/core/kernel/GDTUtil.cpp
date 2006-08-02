/*!
    \file  GDTUtil.cpp
    \brief class functions for set up GDT

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
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
    \author HigePon
    \date   create:2003/06/06 update:
*/
void GDTUtil::setSegDesc(SegDesc* desc, dword base, dword limit, byte type) {

    desc->baseL  = (word)(base & 0xFFFF);
    desc->baseM  = (byte)((base >> 16) & 0xFF);
    desc->baseH  = (byte)((base >> 24) & 0xFF);
    desc->limitL = (word)(limit & 0xFFFF);
    desc->limitH = (byte) (((limit >> 16) & 0x0F) | 0xC0); /* 4KB unit & 32bit segment */
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
void GDTUtil::setSegDescExt(SegDesc* desc, dword base, dword limit, byte type, byte gdbavl) {

    desc->baseL  = (word)(base & 0xFFFF);
    desc->baseM  = (byte)((base >> 16) & 0xFF);
    desc->baseH  = (byte)((base >> 24) & 0xFF);
    desc->limitL = (word)(limit & 0xFFFF);
    desc->limitH = (byte) (((limit >> 16) & 0x0F) | gdbavl);
    desc->type   = type;
    return;
}

/*!
    \brief call lgdt

    \param gdtr gdtr
    \author HigePon
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

    \author HigePon
    \date   create:2002/12/02 update:
*/
void GDTUtil::ltr(word selector) {

    /* ltr */
    asm volatile("ltr %0\n": "=m" (selector));
    return;
}

/*!
    \brief set up GDT

    \author HigePon
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
    setSegDesc(&g_gdt[4], (dword)g_tss, 0x00000067, SEGMENT_PRESENT | SEGMENT_DPL0 | 0x00 | 0x09);

    /* USER CS 0-4GB */
    setSegDesc(&g_gdt[5], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL3 | 0x10 | 0x0A);

    /* USER DS 0-4GB */
    setSegDesc(&g_gdt[6], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL3 | 0x10 | 0x02);

    /* USER SS 0-4GB */
    setSegDesc(&g_gdt[7], 0, 0xFFFFF              , SEGMENT_PRESENT | SEGMENT_DPL3 | 0x10 | 0x02);

#if 1
    /* APM 32bit CS */
    setSegDescExt(&g_gdt[8], g_apmInfo->cs32<<4, (g_apmInfo->cs32_len-1),
			SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A, 0xC0);

    /* APM 16bit CS */
    setSegDescExt(&g_gdt[9], g_apmInfo->cs16<<4, (g_apmInfo->cs16_len-1),
			SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A, 0x80);

    /* APM DS */
    setSegDescExt(&g_gdt[10], g_apmInfo->ds<<4, (g_apmInfo->ds_len-1),
			SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x02, 0xC0);
#endif

#if 0
    /* APM 32bit CS */
    setSegDesc(&g_gdt[8], g_apmInfo->cs32, (g_apmInfo->cs32_len-1),
			SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A);

    /* APM 16bit CS */
    setSegDesc(&g_gdt[9], g_apmInfo->cs16, (g_apmInfo->cs16_len-1),
			SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x0A);

    /* APM DS */
    setSegDesc(&g_gdt[10], g_apmInfo->ds, (g_apmInfo->ds_len-1),
			SEGMENT_PRESENT | SEGMENT_DPL0 | 0x10 | 0x02);
#endif

    /* lgdt */
    GDTR gdtr;
    gdtr.base  = (dword)g_gdt;
    gdtr.limit = sizeof(SegDesc) * GDT_ENTRY_NUM - 1;
    lgdt(&gdtr);

    /* setup TSS */
    setupTSS(0x20);

    return;
}

/*!
    \brief set up TSS

    \author HigePon
    \date   create:2003/07/17 update:2003/08/06
*/
void GDTUtil::setupTSS(word selector) {

    /* prepare dpl0 stack */
    memset(g_tss, 0, sizeof(TSS));
    g_tss->esp0 = 0x90000;
    g_tss->ss0  = KERNEL_SS;

    /* load TSS */
    ltr(selector);

    return;
}

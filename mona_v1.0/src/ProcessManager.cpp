/*!
    \file  ProcessManager.cpp
    \brief class ProcessManager

    class ProcessManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/20 update:$Date$
*/

#include <ProcessManager.h>
#include <monaVga.h>
#include <monaTester.h>
#include <monaLib.h>
#include <monaTester.h>

/*!
    \brief constructor

    initilize process manager

    \author HigePon
    \date   create:2002/12/02 update:
*/
ProcessManager::ProcessManager() {

    /* get address of gdtr */
    sgdt();
}

/*!
    \brief do sgdt

    do sgdt, get start address of gdt

    \author HigePon
    \date   create:2002/12/02 update:
*/
void ProcessManager::sgdt() {

    GDTR gdtr;

    /* set 0 */
    memset(&gdtr, 0, sizeof(gdtr));

    /* sgdt */
    asm volatile("sgdt %0\n": /* no output */: "m" (gdtr));

    /* set start address of gdt */
    gdt_ = (GDT*)gdtr.base;
    return;
}

/*!
    \brief do ltr

    do ltr, load selector value into tr register

    \param selector selector value

    \author HigePon
    \date   create:2002/12/02 update:
*/
inline void ProcessManager::ltr(word selector) const {

    /* ltr */
    asm volatile("ltr %0\n": "=m" (selector));
    return;
}

/*!
    \brief set gdt

    set gdt

    \param gdt   address of gdt to set
    \param base  address of segment start
    \param limit limit size of segment
    \param type  type of segment, see monaKernel.h
    \author HigePon
    \date   create:2002/12/02 update:
*/
void ProcessManager::setGDT(GDT* gdt, dword base, dword limit, byte type) {

    gdt->type   = type;
    gdt->baseL  = base & 0xffff;
    gdt->baseM  = (base >> 16) & 0xff;
    gdt->baseH  = (base >> 24) & 0xff;
    gdt->limitL = limit & 0xffff;
    gdt->limitH = (limit >> 16) & 0xff;
    return;
}

/*!
    \brief set TSS

    set Task State Segment

    \param size size of memory to allocate
    \return address to allocated memory

    \author HigePon
    \date   create:2002/11/21 update:2002/12/03
*/
void ProcessManager::setTSS(TSS* tss, word cs, word ds, void (*f)(), dword eflags
                          , byte* esp, word ss, byte* esp0, word ss0) {

    memset(tss, 0, sizeof(TSS));
    tss->cs     = cs;
    tss->ds     = ds;
    tss->eip    = (byte)f;
    tss->eflags = eflags;
    tss->esp    = (byte)esp;
    tss->ss     = ss;
    tss->esp0   = (byte)esp0;
    tss->ss0    = ss0;
    return;
}

/*!
    \brief print information

    print infomation for debug

    \author HigePon
    \date   create:2002/11/24 update:
*/
void ProcessManager::printInfo() {

    _sys_printf("address of GDT=%d\n", gdt_);

    for (int i = 0; i < GDTNUM; i++) {

        _sys_printf("(%x, %x, %x, %x, %x, %x)\n", gdt_[i].limitL
                                                , gdt_[i].baseL
                                                , gdt_[i].baseM
                                                , gdt_[i].type
                                                , gdt_[i].limitH
                                                , gdt_[i].baseH
                    );
    }

    TSS tss[2];
    byte stack[1][512];
    setTSS(tss + 1, 0x08, 0x10, process1Tester, 0x200, stack[1] + 512, 0x10, 0, 0);
    setGDT(gdt_ + 4, (dword)tss    , sizeof(TSS), TypeTSS);
    setGDT(gdt_ + 5, (dword)tss + 1, sizeof(TSS), TypeTSS);

    for (int i = 0; i < GDTNUM; i++) {

        _sys_printf("(%x, %x, %x, %x, %x, %x)\n", gdt_[i].limitL
                                                , gdt_[i].baseL
                                                , gdt_[i].baseM
                                                , gdt_[i].type
                                                , gdt_[i].limitH
                                                , gdt_[i].baseH
                    );
    }

    return;
}

void ProcessManager::switchProcess() {

}

void ProcessManager::switchProcess(word selector) {

    asm volatile("ljmp %0\n": /* NO OUT PUT */ : "m" (selector));
    return;
}

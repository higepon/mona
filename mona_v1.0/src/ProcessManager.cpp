/*!
    \file  ProcessManager.cpp
    \brief class ProcessManager

    class ProcessManager

    Copyright (c) 2002,2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/20 update:$Date$
*/

#include <ProcessManager.h>
#include <global.h>
#include <string.h>
#include <tester.h>
#include <monaIdt.h>
#include <algorithm>

Process* ProcessManager::current;
Process* ProcessManager::next;

/*!
    \brief constructor

    initilize process manager

    \author HigePon
    \date   create:2002/12/02 update:
*/
ProcessManager::ProcessManager() {

    /* get address of gdtr */
    sgdt();

    /* init first or second process */
    current = &(process_[0]);
    next    = &(process_[1]);
    //    initProcess(process2Tester);
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
    \brief do lldt

    do lldt

    \param address ldtr

    \author HigePon
    \date   create:2002/12/05 update:
*/
inline void ProcessManager::lldt(word address) const {

    /* lldt */
    asm volatile("lldt %0\n": "=m" (address));
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
void ProcessManager::setDT(GDT* gdt, dword base, dword limit, byte type) {

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

    \author HigePon
    \date   create:2002/11/21 update:2002/12/03
*/
void ProcessManager::setTSS(TSS* tss, word cs, word ds, void (*f)(), dword eflags
                          , byte* esp, word ss, byte* esp0, word ss0) {

    memset(tss, 0, sizeof(TSS));
    tss->cs     = cs;
    tss->ds     = ds;
    tss->es     = ds;
    tss->eip    = (dword)f;
    tss->eflags = eflags;
    tss->esp    = (dword)esp;
    tss->ss     = ss;
    tss->esp0   = (dword)esp0;
    tss->ss0    = ss0;
    tss->ldt    = 0x30;
    return;
}

/*!
    \brief multi task test

    multi task test. preemptive

    \sa
    \note
    \author HigePon
    \date   create:2002/12/22 update:
*/
void ProcessManager::multiTaskTester() {


    //g_console->printf("address of GDT=%d\n", gdt_);
    //    printInfo();

    //    setTSS(tss + 1, 0x08, 0x10, process2Tester, 0x200, stack, 0x18, (byte*)0xfff, 0x18);
    setDT(gdt_ + 4, (dword)tss      , sizeof(TSS), SYS_TSS);
    setDT(gdt_ + 5, (dword)(tss + 1), sizeof(TSS), SYS_TSS);
    //    setDT(gdt_ + 6, (dword)(ldt)    , sizeof(GDT), TypeLDT);
    //    setDT(ldt     , (dword)(sss)    , sizeof(GDT), TypeLDT);
    //    setDT(sss     , (dword)(0)      , sizeof(GDT), TypeLDT);

    //    lldt(0x30);
    ltr(0x20);

    /* process start */
    //    process1Tester();

    return;
}

/*!
    \brief print information

    print information about processes

    \author HigePon
    \date   create:2002/12/02 update:2002/12/22
*/
void ProcessManager::printInfo() {

    for (int i = 0; i < GDTNUM; i++) {

        g_console->printf("(%x, %x, %x, %x, %x, %x)\n", gdt_[i].limitL
                                                , gdt_[i].baseL
                                                , gdt_[i].baseM
                                                , gdt_[i].type
                                                , gdt_[i].limitH
                                                , gdt_[i].baseH
                    );
    }
}

/*!
    \brief switch process

    switch process to another

    \param selector selector value

    \author HigePon
    \date   create:2002/12/02 update:
*/
inline void ProcessManager::switchProcess(dword selector) const {

    FARJMP far;
    far.offset   = 0;
    far.selector = selector;
    asm volatile("ljmp %0\n":/* no output */ :"m"(far));
    return;
}

/*!
    \brief set NT flag 1

    set NT flag 1 to switch task by iret

    \author HigePon
    \date   create:2002/12/22 update:
*/
inline void ProcessManager::setNTflag1() const {

    asm volatile("pushf              \n"
                 "pop %%eax          \n"
                 "xor $0x4000, %%eax \n"
                 "push %%eax         \n"
                 "popf               \n"
                 : /* no output */ : /* no input */ : "ax"
                );
}

/*!
    \brief schedule

    scheduling next process

    \author HigePon
    \date   create:2003/01/25 update:
*/
void ProcessManager::schedule() {

    static byte count = 0;
    static byte idx   = 0;
    if (!count) {
        count++;
        return;
    }

    current = next;

    /* now there are only two process */
    //    std::swap(current, next);
    if (idx == 0) {
        next    = &(process_[0]);
    } else if (idx == 1) {
        next    = &(process_[1]);
    } else {
        next    = &(process_[2]);
    }
    idx++;
    if (idx > 2) idx = 0;
    /* switch to next */
    //    _switchProcess(current, next);
}

/*!
    \brief initProcess

    prepare stack of fisrt process
    \param f eip of first process

    \author HigePon
    \date   create:2003/01/25 update:
*/
inline void ProcessManager::initProcess(void (*f)()) {

    process_[0].eax    = 0;
    process_[0].ebx    = 0;
    process_[0].ecx    = 0;
    process_[0].edx    = 0;
    process_[0].edi    = 0;
    process_[0].esi    = 0;
    process_[0].ebp    = (dword*)FIRST_PROCESS_STACK;
    process_[0].esp    = (dword*)FIRST_PROCESS_STACK;
    process_[0].eflags = (dword)0x0200046;
    process_[0].cs     = (dword)0x38;
    process_[0].eip    = (dword)f;

    process_[1].eax    = 0;
    process_[1].ebx    = 0;
    process_[1].ecx    = 0;
    process_[1].edx    = 0;
    process_[1].edi    = 0;
    process_[1].esi    = 0;
    process_[1].ebp    = (dword*)FIRST_PROCESS_STACK;
    process_[1].esp    = (dword*)FIRST_PROCESS_STACK;
    process_[1].eflags = (dword)0x0200046;
    process_[1].cs     = (dword)0x38;
    process_[1].eip    = (dword)f;

    process_[2].eax    = 0;
    process_[2].ebx    = 0;
    process_[2].ecx    = 0;
    process_[2].edx    = 0;
    process_[2].edi    = 0;
    process_[2].esi    = 0;
    process_[2].ebp    = (dword*)15000;
    process_[2].esp    = (dword*)15000;
    process_[2].eflags = (dword)0x0200046;
    process_[2].cs     = (dword)0x38;
    //    process_[2].eip    = (dword)process3Tester;
}

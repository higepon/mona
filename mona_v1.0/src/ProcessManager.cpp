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
#include <monaVga.h>
#include <monaTester.h>
#include <string.h>
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

    taskidx_ = 0;

    /* init first or second process */
    initProcess(process2Tester);
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


    _sys_printf("address of GDT=%d\n", gdt_);

    //    printInfo();

    setTSS(tss + 1, 0x08, 0x10, process2Tester, 0x200, stack, 0x18, (byte*)0xfff, 0x18);
    setDT(gdt_ + 4, (dword)tss      , sizeof(TSS), SYS_TSS);
    setDT(gdt_ + 5, (dword)(tss + 1), sizeof(TSS), SYS_TSS);
    //    setDT(gdt_ + 6, (dword)(ldt)    , sizeof(GDT), TypeLDT);
    //    setDT(ldt     , (dword)(sss)    , sizeof(GDT), TypeLDT);
    //    setDT(sss     , (dword)(0)      , sizeof(GDT), TypeLDT);

    //    lldt(0x30);
    ltr(0x20);

    /* process start */
    process1Tester();

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

        _sys_printf("(%x, %x, %x, %x, %x, %x)\n", gdt_[i].limitL
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

    switch process to next process

    \author HigePon
    \date   create:2002/12/02 update:2003/01/14
*/
inline void ProcessManager::switchProcess() {

    static dword current = 0x20;

    if (taskidx_ == 0) {
        taskidx_++;
        return;
    }

    if (current == 0x20) {
        tss[0].backlink = 0x28;
        setDT(gdt_ + 5, (dword)(tss + 1), sizeof(TSS), SYS_TSS_BUSY);
        current = 0x28;
    } else {
        tss[1].backlink = 0x20;
        setDT(gdt_ + 4, (dword)tss      , sizeof(TSS), SYS_TSS_BUSY);
        current = 0x20;
    }

    /* set NT flag 1 */
    setNTflag1();

    asm volatile("iret");
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
    \brief scheduling function

    this function re-scheduling running process

    \author HigePon
    \date   create:2003/01/10 update:
*/
void ProcessManager::schedule() {
    switchProcess2();
}

static dword* esp1;
static dword* esp2 = (dword*)0x9884;
static int prev = 2;

/*!
    \brief switch process2 with out TSS

    switch process to next process(without TSS)

    \author HigePon
    \date   create:2002/12/02 update:2003/01/14
*/
inline void ProcessManager::switchProcess2() {

    if (taskidx_ == 0) {
        taskidx_++;
        return;
    }

    if (prev == 2) {
        prev = 1;
        asm volatile("pusha         \n"
                     "pushf         \n"
                     "mov %%esp, %0 \n"
                     "mov %1, %%esp \n"
                     "popf          \n"
                     "popa          \n"
                     : "=m" (esp1)
                     : "m" (esp2)
                    );
    } else {

        prev = 2;
        asm volatile("pusha         \n"
                     "pushf         \n"
                     "mov %%esp, %0 \n"
                     "mov %1, %%esp \n"
                     "popf          \n"
                     "popa          \n"
                     : "=m" (esp2)
                     : "m" (esp1)
                    );
    }
}

inline void ProcessManager::initProcess(void (*f)()) {

    *(--esp2) = (dword)f;
    *(--esp2) = (dword)0x08;
    *(--esp2) = (dword)0x00000046; /* for popf */
    esp2 -= 8;                     /* for popa */
}

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
    \brief set TSS

    set Task State Segment

    \param size size of memory to allocate
    \return address to allocated memory

    \author HigePon
    \date   create:2002/11/21 update:
*/
void ProcessManager::setTSS(TSS* tss, word cs, word ds, void (*f)(), dword eflags, byte* esp, word ss, byte* esp0, word ss0) {

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
    return;
}

void ProcessManager::switchProcess() {

}

void ProcessManager::switchProcess(word selector) {

    asm volatile("ljmp %0\n": /* NO OUT PUT */ : "m" (selector));
    return;
}

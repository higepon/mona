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

    int from = 0x02;
    int to   = 0xfe;
    _sys_printf("from = %d to = %d\n", from, to);
    asm volatile("mov %1, %%ax\n"
                 "mov %%ax, %0\n"
                 : "=m" (to): "m" (from));
    _sys_printf("from = %d to = %d\n", from, to);
    return;
}

void ProcessManager::switchProcess() {

}

void ProcessManager::switchProcess(word selector) {

}

/*!
    \file  ProcessManager.cpp
    \brief class ProcessManager

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/06 update:$Date$
*/

#include <ProcessManager.h>

ProcessManager::ProcessManager() {

    pid_ = 0;
}

void ProcessManager::switchTo(ProcessInfo* pinfo){

    g_current_process = pinfo;

    arch_switch_process();
}

void ProcessManager::schedule(){

    scheduler_.schedule();
    switchTo(g_current_process);

}

virtual_addr ProcessManager::allocateStack() {

    return 0x800000;
}

PTE* ProcessManager::allocatePageDir() {


    return g_page_dir;
}

dword ProcessManager::allocatePID() {

    pid_++;
    return pid_;
}


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
    scheduler_ = new Scheduler();
}

void ProcessManager::switchProcess() {

    info(DUMP, "switch Process");
    arch_switch_process();
}

void ProcessManager::schedule(){

    scheduler_->schedule();
    this->switchProcess();

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

bool ProcessManager::addProcess(Process* process, virtual_addr entry) {

    process->setup(entry, allocateStack(), allocatePageDir(), allocatePID());


}

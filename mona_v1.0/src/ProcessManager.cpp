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


    info(DUMP, "esp=%x pid=%x eip=%x eflags=%x cs=%x", g_current_process->esp, g_current_process->pid
             , g_current_process->eip, g_current_process->eflags, g_current_process->cs);


    if (scheduler_->toUserMode()) {

        info(DEV_NOTICE, "to user mode\n");
        arch_switch_process_to_user_mode();

    } else {

        info(DEV_NOTICE, "to same mode");
        arch_switch_process();
    }
}

void ProcessManager::schedule(){

    scheduler_->schedule();
    this->switchProcess();

}

virtual_addr ProcessManager::allocateStack() {

    static int i = 0;

    i++;

    return 0x80000 + i * 4096;
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
    scheduler_->addToPrev(&(process->pinfo_));

}

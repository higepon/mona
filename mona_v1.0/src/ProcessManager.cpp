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
    //    Process* tmp = new Process("first");
    //    g_current_process = &(tmp->pinfo_);
}

void ProcessManager::switchProcess() {

    g_console->printf("switch Process");

    info(DUMP, "esp=%x pid=%x eip=%x eflags=%x cs=%x", g_current_process->esp, g_current_process->pid
             , g_current_process->eip, g_current_process->eflags, g_current_process->cs);

    arch_switch_process();
}

void ProcessManager::schedule(){

    scheduler_->schedule();
    this->switchProcess();

}

virtual_addr ProcessManager::allocateStack() {

    return 0x80000;
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

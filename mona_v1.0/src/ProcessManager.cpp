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

    pid_  = 0;
    pnum_ = 0;
    scheduler_ = new Scheduler();
}

void ProcessManager::switchProcess() {


    info(DUMP, "esp=%x pid=%x eip=%x eflags=%x cs=%x ss=%x\n", g_current_process->esp, g_current_process->pid
             , g_current_process->eip, g_current_process->eflags, g_current_process->cs, g_current_process->ss);


    if (scheduler_->toUserMode()) {

        info(DEV_NOTICE, "to user mode\n");
        g_current_process->ss = 0x3b;
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

    return 0x70000 + i * 4096;
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

    g_process[pnum_] = process;
    pnum_++;
}

void ProcessManager::printOneProcess(ProcessInfo* info) const {

    g_console->printf("[%s]\n", info->name);

}

void ProcessManager::printAllProcesses() const {

    for (dword i = 0; i < pnum_; i++) {
        printOneProcess(&(g_process[i]->pinfo_));
    }
}

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
#include <io.h>
#include <SystemInfo.h>

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

        g_tss->esp0 = g_current_process->esp0;
        g_tss->ss0  = g_current_process->ss0;
        arch_switch_process_to_user_mode();

    } else {

        info(DEV_NOTICE, "to same mode");
        arch_switch_process();
    }
}

void ProcessManager::schedule(){

    g_current_process->tick += 10;
    g_process_manager->tick();
    scheduler_->schedule();
    g_current_process->state = Process::RUNNING;

    this->switchProcess();

}

virtual_addr ProcessManager::allocateStack() {

    static int i = 0;

    i++;

    return 0x70000 + i * 4096;
}

virtual_addr ProcessManager::allocateKernelStack(dword dpl) {

    if (dpl == DPL_KERNEL) return 0;

    static int i = 0;

    i++;

    return 0x100000 + i * 4096;
}

PTE* ProcessManager::allocatePageDir() {


    return g_page_dir;
}

dword ProcessManager::allocatePID() {

    pid_++;
    return pid_;
}

bool ProcessManager::addProcess(Process* process, virtual_addr entry) {

    process->setup(entry, allocateStack(), allocateKernelStack(process->pinfo_.dpl), allocatePageDir(), allocatePID());

    scheduler_->addProcess(&(process->pinfo_));

    process->pinfo_.state = Process::READY;

    g_process[pnum_] = process;
    pnum_++;
}

void ProcessManager::printOneProcess(ProcessInfo* info) const {

    char* state;

    if (info->state == Process::RUNNING)       state = "Running ";
    else if (info->state == Process::SLEEPING) state = "Sleeping";
    else if (info->state == Process::READY)    state = "Ready   ";

    g_console->printf("|%s|  %d  |  %d  |%x|%x|%x| %s |%d\n", info->name, info->pid, info->dpl, info->cs, info->ss, info->esp, state, info->tick);

}

void ProcessManager::printAllProcesses() const {

    g_console->printf("total tick=%d                                \n", g_process_manager->getTick());
    g_console->printf("|    name     | pid | dpl |    cs    |    ss    |    esp   |   state  |tick\n");
    g_console->printf("---------------------------------------------------------------------------\n");

    for (dword i = 0; i < pnum_; i++) {
        printOneProcess(&(g_process[i]->pinfo_));
    }
}

void ProcessManager::tick() {

    scheduler_->tick();
}

dword ProcessManager::getTick() const {

    return scheduler_->getTick();
}

void ProcessManager::sleep(ProcessInfo* process, dword tick) {

    process->state = Process::SLEEPING;
    scheduler_->sleep(process, tick);
}

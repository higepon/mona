/*!
    \file  ProcessManager.cpp
    \brief class ProcessManager

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/06 update:$Date$
*/

#include <ProcessManager.h>
#include <io.h>
#include <SystemInfo.h>
#include <syscalls.h>

ProcessManager::ProcessManager(Process* idle) {

    pid_  = 0;
    pnum_ = 0;
    idle->setup((dword)idle_process, allocateStack(), allocateKernelStack(idle->pinfo_.dpl), allocatePageDir(), allocatePID());
    scheduler_ = new Scheduler(idle);
}

void ProcessManager::switchProcess() {

    info(DEBUG, "[ name=%s  esp0=%x eip=%x cs=%x eflags=%x esp=%x ds=%x ss=%x ss0=%x]\n"
         , g_current_process->name
         , g_current_process->esp0
         , g_current_process->eip
         , g_current_process->cs
         , g_current_process->eflags
         , g_current_process->esp
         , g_current_process->ds
         , g_current_process->ss
         , g_current_process->ss0
         );

    /* switch to user process */
    if ((g_current_process->cs & DPL_USER) == DPL_USER) {

        /* user mode need kernel stack */
        g_tss->esp0 = g_current_process->esp0;
        g_tss->ss0  = g_current_process->ss0;

        /* V86 mode. not completed */
        if ((g_current_process->eflags & 0x20000) == 0x20000) {

            g_console->printf("to v86\n");
            arch_switch_process_to_v86_mode();
        }
        else arch_switch_process_to_user_mode();

    } else {

        info(DEBUG, "TO KERNEL\n");
        arch_switch_process();
    }

    /* does't come here */
}

void ProcessManager::schedule(){

    /* process tick */
    g_current_process->tick += 10;

    /* kernel tick */
    g_process_manager->tick();

    scheduler_->schedule();

    g_current_process->state = Process::RUNNING;

    /* switch to next */
    this->switchProcess();
}

virtual_addr ProcessManager::allocateStack() {

    return 0xFFFFFFFF;
}

virtual_addr ProcessManager::allocateKernelStack(dword dpl) {

    static int i = 0;

    /* kernel mode process doesn't need kernel stack */
    if (dpl == DPL_KERNEL) return 0;

    i++;

    return 0x100000 + i * 4096;
}

PageEntry* ProcessManager::allocatePageDir() {

    return g_page_manager->createNewPageDirectory();
}

dword ProcessManager::allocatePID() {

    pid_++;
    return pid_;
}

bool ProcessManager::addProcess(Process* process, virtual_addr entry) {

    return addProcess(process, allocatePageDir(), entry);
}

bool ProcessManager::addProcess(Process* process, PageEntry* directory, virtual_addr entry) {

    process->setup(entry, allocateStack(), allocateKernelStack(process->pinfo_.dpl), directory, allocatePID());

    process->pinfo_.state = Process::READY;

    process->pinfo_.stack  = new StackSegment(0xFFFFEFFF, 0x1000, 0x3000);
    g_page_manager->allocatePhysicalPage((PageEntry*)(process->pinfo_.cr3), 0xFFFFFFFF, true, true, true);

    g_console->printf("esp=[%s, %x]", process->pinfo_.name, process->pinfo_.esp);

    g_process[pnum_] = process;
    pnum_++;

    enter_kernel_lock_mode();
    scheduler_->addProcess(&(process->pinfo_));
    exit_kernel_lock_mode();
    return true;
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
        if (g_process[i]) printOneProcess(&(g_process[i]->pinfo_));
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

bool ProcessManager::kill(ProcessInfo* process) {

    g_console->printf("kill called");

    for (dword i = 0; i < pnum_; i++) {

        if (g_process[i] == process->process) {

            g_process[i] = NULL;
            break;
        }
    }

    scheduler_->kill(process);

    scheduler_->schedule();
    g_current_process->state = Process::RUNNING;

    this->switchProcess();
    return true;
}

ProcessInfo* ProcessManager::findProcess(const char* name) {

    return scheduler_->findProcess(name);
}

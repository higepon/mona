/*!
    \file  Process.cpp
    \brief class Process

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/

#include <global.h>
#include <PageManager.h>

extern "C" Process** g_process;

const int Process::RUNNING  = 0;
const int Process::READY    = 1;
const int Process::SLEEPING = 2;

/*!
    \brief setup

    \author HigePon
    \date   create:2003/06/28 update:
*/
void Process::setup() {

    g_process = new Process*[MAX_PROCESS];

    Process* kernel = new Process("Kernel");
    g_current_process = &(kernel->pinfo_);

    return;
}

Process::Process(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = KERNEL_CS;
    pinfo_.ds      = KERNEL_DS;
    pinfo_.es      = KERNEL_DS;
    pinfo_.ss      = KERNEL_SS;
    pinfo_.eflags  = 0x200;
    pinfo_.eax     = 0;
    pinfo_.ecx     = 0;
    pinfo_.edx     = 0;
    pinfo_.ebx     = 0;
    pinfo_.esi     = 0;
    pinfo_.edi     = 0;
    pinfo_.tick    = 0;
    pinfo_.dpl     = DPL_KERNEL;

    pinfo_.shared      = new SharedMemorySegment();
    pinfo_.heap        = new HeapSegment(0xC0000000, 1024 * 1024); /* 1MB */
    pinfo_.messageList = new HList<Message*>(3, 5);
    QueueManager::init(pinfo_.shared);

}

Process::~Process() {

    if (pinfo_.shared) {
        delete(pinfo_.shared);
    }

    if (pinfo_.heap) {
        delete(pinfo_.heap);
    }

    if (pinfo_.messageList) {
        delete(pinfo_.messageList);
    }
}

void Process::setup(virtual_addr entryPoint, virtual_addr stack, virtual_addr kernel_stack, PageEntry* pagedir, dword pid) {

    pinfo_.eip  = (dword)entryPoint;
    pinfo_.esp  = (dword)stack;
    pinfo_.ebp  = (dword)stack;
    pinfo_.cr3  = (dword)pagedir;
    pinfo_.pid  = pid;
    pinfo_.ss0  = KERNEL_SS;
    pinfo_.esp0 = (dword)kernel_stack;
    return;
}

UserProcess::UserProcess(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = USER_CS;
    pinfo_.ds      = USER_DS;
    pinfo_.es      = USER_DS;
    pinfo_.ss      = USER_SS;
    pinfo_.eflags  = 0x200;
    pinfo_.eax     = 0;
    pinfo_.ecx     = 0;
    pinfo_.edx     = 0;
    pinfo_.ebx     = 0;
    pinfo_.esi     = 0;
    pinfo_.edi     = 0;
    pinfo_.tick    = 0;
    pinfo_.dpl     = DPL_USER;

    pinfo_.shared      = new SharedMemorySegment();
    pinfo_.heap        = new HeapSegment(0xC0000000, 1024 * 1024); /* 1MB */
    pinfo_.messageList = new HList<Message*>(3, 5);
    QueueManager::init(pinfo_.shared);
}

V86Process::V86Process(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = USER_CS;
    pinfo_.ds      = USER_DS;
    pinfo_.es      = USER_DS;
    pinfo_.ss      = USER_SS;
    pinfo_.eflags  = 0x20200;
    pinfo_.eax     = 0;
    pinfo_.ecx     = 0;
    pinfo_.edx     = 0;
    pinfo_.ebx     = 0;
    pinfo_.esi     = 0;
    pinfo_.edi     = 0;
    pinfo_.tick    = 0;
    pinfo_.dpl     = DPL_USER;
}

ProcessScheduler::ProcessScheduler(Process_* idle) {

    /* list of process */
    list_ = new HList<Process_*>();
    checkMemoryAllocate(list_, "ProcessScheduler memory allocate list");

    /* idle process */
    idle_ = idle;
}

ProcessScheduler::~ProcessScheduler() {

    delete list_;
}

Process_* ProcessScheduler::schedule(Process_* current) {

    /* check dispach is empty */
    if (list_->isEmpty()) return idle_;

    /* round robin */
    list_->add(current);
    return (list_->remove(0));
}

int ProcessScheduler::addProcess(Process_* process) {

    list_->add(process);
    return NORMAL;
}

int ProcessScheduler::kill(Process_* process) {

    list_->remove(process);
    return NORMAL;
}


/*********************************************************

    not yet

********************************************************/
/*----------------------------------------------------------------------
    ProcessManager
----------------------------------------------------------------------*/
ProcessManager_::ProcessManager_(PageManager* pageManager) {

    /* create idle process */
    idle_ = new KernelProcess_("Idle");
    checkMemoryAllocate(idle_, "ProcessManager idle memory allcate");

    /* scheduler */
    scheduler_ = new ProcessScheduler(idle_);
    checkMemoryAllocate(scheduler_, "ProcessManager memory allocate scheduler");

    /* page manager */
    pageManager_ = pageManager;
}

ProcessManager_::~ProcessManager_() {

    /* destroy */
    delete scheduler_;
    delete idle_;
}

int ProcessManager_::kill(Process_* process) {

    int result;

    /* remove from scheduler */
    if (NORMAL != (result = scheduler_->kill(process))) {
        return result;
    }

    /* destroy address space of proces */

    /* delete process */
    delete process;

    return NORMAL;
}

int ProcessManager_::switchProcess() {

    /* do nothing ??? */

    return NORMAL;
}

Process_* ProcessManager_::schedule() {
    return scheduler_->schedule(current_);
}

Process_* ProcessManager_::createProcess(int type, const char* name) {

    Process_* result;

    /* process type */
    switch (type) {

      case USER_PROCESS:
          result = new UserProcess_(name);
          break;
      case KERNEL_PROCESS:
          result = new KernelProcess_(name);
          break;
      default:
          result = (Process_*)NULL;
          break;
    }

    return result;
}

Process_* ProcessManager_::getCurrentProcess() const {
    return current_;
}

int ProcessManager_::addProcess(Process_* process) {
    return (scheduler_->addProcess(process));
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
Process_::Process_(const char* name) {

    /* name */
    strncpy(name_, name, sizeof(name_));
}

Process_::~Process_() {
}

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
UserProcess_::UserProcess_(const char* name) : Process_(name) {

    /* not kernel mode */
    isKernelMode_ = false;
}

UserProcess_::~UserProcess_() {
}

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
KernelProcess_::KernelProcess_(const char* name) : Process_(name) {

    /* kernel mode */
    isKernelMode_ = true;
}

KernelProcess_::~KernelProcess_() {
}

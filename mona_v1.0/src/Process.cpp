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

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
Thread::Thread(dword programCounter) : tick_(0), timeLeft_(4) {

    /* thread information */
    threadInfo_ = (ThreadInfo*)malloc(sizeof(ThreadInfo));
    checkMemoryAllocate(threadInfo_, "class Thread info allocate");

    /* thread information arch dependent */
    threadInfo_->archinfo = (ArchThreadInfo*)malloc(sizeof(ArchThreadInfo));
    checkMemoryAllocate(threadInfo_->archinfo, "class Thread arch info allocate");

    /* programCounter */
    setProgramCounter(programCounter);
}

Thread::~Thread() {

    /* free memory */
    free(threadInfo_->archinfo);
    free(threadInfo_);
}

/*----------------------------------------------------------------------
    ThreadManager
----------------------------------------------------------------------*/
ThreadManager::ThreadManager(bool isUser, PageManager* pageManager) {

    /* how? idle       */
    /*                 */
    /* not implemented */
    /*                 */

    /* user or kernel */
    isUser_ = isUser;

    /* scheduler */
    scheduler_ = new ThreadScheduler();
    checkMemoryAllocate(scheduler_, "ThreadManager scheduler memory allocate");

    /* page manager */
    pageManager_ = pageManager;
}

ThreadManager::~ThreadManager() {
}

Thread* ThreadManager::create(dword programCounter) {

    Thread* thread = new Thread(programCounter);

    /* arch dependent */
    if (isUser_) {
        archCreateUserThread(thread);
    } else {
        archCreateThread(thread);
    }

    /*                 */
    /* not implemented */
    /*                 */
    return (Thread*)NULL;
}

void ThreadManager::archCreateThread(Thread* thread) const {

    ThreadInfo* info      = thread->getThreadInfo();
    ArchThreadInfo* ainfo = info->archinfo;
    ainfo->cs      = KERNEL_CS;
    ainfo->cs      = KERNEL_CS;
    ainfo->ds      = KERNEL_DS;
    ainfo->es      = KERNEL_DS;
    ainfo->ss      = KERNEL_SS;
    ainfo->eflags  = 0x200;
    ainfo->eax     = 0;
    ainfo->ecx     = 0;
    ainfo->edx     = 0;
    ainfo->ebx     = 0;
    ainfo->esi     = 0;
    ainfo->edi     = 0;
    ainfo->dpl     = DPL_KERNEL;
}

void ThreadManager::archCreateUserThread(Thread* thread) const {

    ThreadInfo* info      = thread->getThreadInfo();
    ArchThreadInfo* ainfo = info->archinfo;
    ainfo->cs      = USER_CS;
    ainfo->ds      = USER_DS;
    ainfo->es      = USER_DS;
    ainfo->ss      = USER_SS;
    ainfo->eflags  = 0x200;
    ainfo->eax     = 0;
    ainfo->ecx     = 0;
    ainfo->edx     = 0;
    ainfo->ebx     = 0;
    ainfo->esi     = 0;
    ainfo->edi     = 0;
    ainfo->dpl     = DPL_USER;
}

int ThreadManager::join(Thread* thread) {
    return scheduler_->join(thread);
}

int ThreadManager::kill(Thread* thread) {
    return scheduler_->kill(thread);
}

int ThreadManager::switchThread() {

    /*                 */
    /* not implemented */
    /*                 */
    /* arch_xxxxx      */

    return NORMAL;
}

Thread* ThreadManager::schedule() {
    return scheduler_->schedule(current_);
}
/*----------------------------------------------------------------------
    ThreadScheduler
----------------------------------------------------------------------*/
ThreadScheduler::ThreadScheduler() {

    /* list of thread */
    list_ = new HList<Thread*>();
    checkMemoryAllocate(list_, "ThreadScheduler memory allocate list");
}

ThreadScheduler::~ThreadScheduler() {
    delete list_;
}

Thread* ThreadScheduler::schedule(Thread* current) {

    /* tick */
    current->tick();

    /* thread has time yet */
    if (current->hasTimeLeft()) {
        return current;
    }

    /* check dispach list is empty */
    if (list_->isEmpty()) {
        return current;
    }

    /* round robin */
    list_->add(current);
    return (list_->removeAt(0));
}

int ThreadScheduler::join(Thread* thread) {

    list_->add(thread);
    return NORMAL;
}

int ThreadScheduler::kill(Thread* thread) {

    list_->remove(thread);
    return NORMAL;
}

/*----------------------------------------------------------------------
    ProcessScheduler
----------------------------------------------------------------------*/
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

    /* tick */
    current->tick();

    /* process has time yet */
    if (current->hasTimeLeft()) return current;

    /* check dispach list is empty */
    if (list_->isEmpty()) return idle_;

    /* round robin */
    if (current != idle_) {
        list_->add(current);
    }
    return (list_->removeAt(0));
}

int ProcessScheduler::add(Process_* process) {

    list_->add(process);
    return NORMAL;
}

int ProcessScheduler::kill(Process_* process) {

    list_->remove(process);
    return NORMAL;
}

bool ProcessScheduler::hasProcess(Process_* process) const {

    /* check has process */
    return list_->hasElement(process);
}

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

int ProcessManager_::join(Process_* process, Thread* thread) {

    /* check process */
    if (!hasProcess(process)) {
        return ERROR;
    }

    /* join */
    return process->join(thread);
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

bool ProcessManager_::schedule() {

    bool      isProcessChanged;
    Process_* next;

    /* schdule */
    next = scheduler_->schedule(current_);

    /* next is current ? */
    isProcessChanged = (next != current_);
    current_         = next;
    return isProcessChanged;
}

Process_* ProcessManager_::create(int type, const char* name) {

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

int ProcessManager_::add(Process_* process) {

    /* is this first process */
    if (current_ == NULL) current_ = process;
    return (scheduler_->add(process));
}

Thread* ProcessManager_::createThread(Process_* process, dword programCounter) {

    /* check process */
    if (!hasProcess(process)) {
        return (Thread*)NULL;
    }

    /* create thread */
    return process->createThread(programCounter);
}

bool ProcessManager_::hasProcess(Process_* process) const {
    return scheduler_->hasProcess(process);
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
Process_::Process_(const char* name) : tick_(0), timeLeft_(4) {

    /* name */
    strncpy(name_, name, sizeof(name_));
}

Process_::~Process_() {
}

int Process_::join(Thread* thread) {
    return threadManager_->join(thread);
}

Thread* Process_::createThread(dword programCounter) {
    return threadManager_->create(programCounter);
}

Thread* Process_::schedule() {
    return threadManager_->schedule();
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

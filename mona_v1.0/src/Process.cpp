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
Thread::Thread() : tick_(0), timeLeft_(4) {

    /* thread information */
    threadInfo_ = new ThreadInfo;
    checkMemoryAllocate(threadInfo_, "class Thread info allocate");

    /* thread information arch dependent */
    threadInfo_->archinfo = new ArchThreadInfo;
    checkMemoryAllocate(threadInfo_->archinfo, "class Thread arch info allocate");
}

Thread::~Thread() {

    /* free memory */
    free(threadInfo_->archinfo);
    free(threadInfo_);
}

/*----------------------------------------------------------------------
    ThreadManager
----------------------------------------------------------------------*/
ThreadManager::ThreadManager(bool isUser) : threadCount(0) {

    /* user or kernel */
    isUser_ = isUser;

    /* list of thread */
    dispatchList_ = new HList<Thread*>();
    checkMemoryAllocate(dispatchList_, "ThreadManager memory allocate list");
}

ThreadManager::~ThreadManager() {

    delete dispatchList_;
}

Thread* ThreadManager::create(dword programCounter, PageEntry* pageDirectory) {

    Thread* thread = new Thread();
    threadCount++;

    /* arch dependent */
    if (isUser_) {
        archCreateUserThread(thread, programCounter, pageDirectory);
    } else {
        archCreateThread(thread, programCounter, pageDirectory);
    }
    return thread;
}

void ThreadManager::archCreateThread(Thread* thread, dword programCounter, PageEntry* pageDirectory) const {

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
    ainfo->esp     = allocateStack();
    ainfo->ebp     = allocateStack();
    ainfo->eip     = programCounter;
    ainfo->cr3     = (PhysicalAddress)pageDirectory;
}

void ThreadManager::archCreateUserThread(Thread* thread, dword programCounter, PageEntry* pageDirectory) const {

    ThreadInfo* info      = thread->getThreadInfo();
    ArchThreadInfo* ainfo = info->archinfo;
    ainfo->cs      = USER_CS;
    ainfo->ds      = USER_DS;
    ainfo->es      = USER_DS;
    ainfo->ss      = USER_SS;
    ainfo->ss0     = KERNEL_SS;
    ainfo->eflags  = 0x200;
    ainfo->eax     = 0;
    ainfo->ecx     = 0;
    ainfo->edx     = 0;
    ainfo->ebx     = 0;
    ainfo->esi     = 0;
    ainfo->edi     = 0;
    ainfo->dpl     = DPL_USER;
    ainfo->esp     = allocateStack();
    ainfo->ebp     = allocateStack();
    ainfo->esp0    = g_processManager->allocateKernelStack();
    ainfo->eip     = programCounter;
    ainfo->cr3     = (PhysicalAddress)pageDirectory;
}

int ThreadManager::join(Thread* thread) {
    dispatchList_->add(thread);
    return NORMAL;
}

int ThreadManager::kill(Thread* thread) {
    dispatchList_->remove(thread);
    delete thread; /* ? */
    return NORMAL;
}

int ThreadManager::switchThread() {

    /*                 */
    /* not implemented */
    /*                 */
    /* arch_xxxxx      */

    return NORMAL;
}

Thread* ThreadManager::schedule() {
    //    return scheduler_->schedule(current_);
    return (Thread*)NULL;
}

/*----------------------------------------------------------------------
    Idle Thread
----------------------------------------------------------------------*/
void idleThread() {
    for (;;);
}
/*----------------------------------------------------------------------
    ProcessManager
----------------------------------------------------------------------*/
ProcessManager_::ProcessManager_(PageManager* pageManager) : current_(NULL) {

    /* page manager */
    pageManager_ = pageManager;

    /* create idle process */
    idle_ = new KernelProcess_("Idle", pageManager_->createNewPageDirectory());
    checkMemoryAllocate(idle_, "ProcessManager idle memory allcate");
    add(idle_);

    /* create thread for idle process */
    join(idle_, createThread(idle_, (dword)idleThread));

    /* dispach list */
    dispatchList_ = new HList<Process_*>();
    checkMemoryAllocate(dispatchList_, "ProcessManager dispatch list");
}

ProcessManager_::~ProcessManager_() {

    /* destroy */
    delete dispatchList_;
    delete idle_;
}

int ProcessManager_::join(Process_* process, Thread* thread) {

    bool wait;

    /* check process */
    if (!dispatchList_->hasElement(process)
        && !(wait = waitList_->hasElement(process))
        && process != idle_) {
        return ERROR;
    }

    /* join */
    process->join(thread);

    /* regist to dispatchList */
    if (wait) {
        waitList_->remove(process);
        dispatchList_->add(process);
    }

    return NORMAL;
}

int ProcessManager_::kill(Process_* process) {

    /* remove from list */
    dispatchList_->remove(process);
    waitList_->remove(process);

    /* delete process */
    delete process;

    return NORMAL;
}

int ProcessManager_::switchProcess() {

    /* do nothing ??? */

    return NORMAL;
}

LinearAddress ProcessManager_::allocateKernelStack() const {
    static int i = 0;
    return 0x100000 + i * 4096;
}

bool ProcessManager_::schedule() {

    bool      isProcessChanged;
    Process_* next;

    /* tick */
    current_->tick();

    /* process has time yet */
    if (current_->hasTimeLeft()) {

        /* next is current */
        return false;
    }

    /* only idle? */
    if (current_ == idle_ && dispatchList_->isEmpty()) {

        /* next is idle */
        return false;
    }

    /* round robin */
    if (current_ != idle_) {
        dispatchList_->add(current_);
    }
    next = dispatchList_->removeAt(0);
    isProcessChanged = next != current_;
    current_ = next;
    return isProcessChanged;
}

Process_* ProcessManager_::create(int type, const char* name) {

    Process_* result;

    /* process type */
    switch (type) {

      case USER_PROCESS:
          result = new UserProcess_(name, pageManager_->createNewPageDirectory());
          break;
      case KERNEL_PROCESS:
          result = new KernelProcess_(name, pageManager_->createNewPageDirectory());
          break;
      default:
          result = (Process_*)NULL;
          break;
    }

    return result;
}

int ProcessManager_::add(Process_* process) {

    /* process has Thread? */
    if (process->hasActiveThread()) {

        /* dispatch */
        dispatchList_->add(process);

        /* first process of kernel */
        if (current_ == NULL) {
            current_ = process;
        }

    /* process has not thread yet */
    } else {
        waitList_->add(process);
    }

    return NORMAL;
}

Thread* ProcessManager_::createThread(Process_* process, dword programCounter) {

    /* check process */
    if (!dispatchList_->hasElement(process)) {
        return (Thread*)NULL;
    }

    /* create thread */
    return process->createThread(programCounter);
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
Process_::Process_(const char* name, PageEntry* directory) : tick_(0), timeLeft_(4) {

    /* name */
    strncpy(name_, name, sizeof(name_));

    /* address space */
    pageDirectory_ = directory;
}

Process_::~Process_() {
}

int Process_::join(Thread* thread) {
    return threadManager_->join(thread);
}

Thread* Process_::createThread(dword programCounter) {
    return threadManager_->create(programCounter, pageDirectory_);
}

Thread* Process_::schedule() {
    return threadManager_->schedule();
}

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
UserProcess_::UserProcess_(const char* name, PageEntry* directory) : Process_(name, directory) {

    /* not kernel mode */
    isUserMode_ = true;

    /* thread manager */
    threadManager_ = new ThreadManager(isUserMode_);
}

UserProcess_::~UserProcess_() {
}

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
KernelProcess_::KernelProcess_(const char* name, PageEntry* directory) : Process_(name, directory) {

    /* kernel mode */
    isUserMode_ = false;

    /* thread manager */
    threadManager_ = new ThreadManager(isUserMode_);
}

KernelProcess_::~KernelProcess_() {
}

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
#include <Process.h>
#include <PageManager.h>

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
Thread::Thread() : tick_(0), timeLeft_(1) {

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
ThreadManager::ThreadManager(bool isUser) : current_(NULL), threadCount(0) {

    /* user or kernel */
    isUser_ = isUser;

    /* list of thread */
    dispatchList_ = new HList<Thread*>();
    waitList_     = new HList<Thread*>();
    checkMemoryAllocate(dispatchList_, "ThreadManager memory allocate list");
}

ThreadManager::~ThreadManager() {

    delete dispatchList_;
    delete waitList_;
}

void ThreadManager::setup() {

    static Thread thread;
    g_currentThread = thread.getThreadInfo();
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
    if (current_ == NULL) {
        current_ = thread;
    }
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

    current_->tick();

    /* thread has time yet */
    if (current_->hasTimeLeft()) {

        /* next is current */
        return current_;
    }

    /* round robin */
    current_->setTimeLeft(1);
    Thread* tmp = dispatchList_->removeAt(0);
    dispatchList_->add(tmp);
    current_ = dispatchList_->get(0);
    return current_;
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
ProcessManager::ProcessManager(PageManager* pageManager) {

    /* page manager */
    pageManager_ = pageManager;

    /* dispach and wait list */
    dispatchList_ = new HList<Process*>();
    waitList_     = new HList<Process*>();
    checkMemoryAllocate(dispatchList_, "ProcessManager dispatch list");
    checkMemoryAllocate(waitList_    , "ProcessManager wait     list");

    /* create idle process */
    idle_ = new KernelProcess("Idle", pageManager_->createNewPageDirectory());
    checkMemoryAllocate(idle_, "ProcessManager idle memory allcate");
    add(idle_);
    current_ = idle_;

    /* create thread for idle process */
    Thread* thread = createThread(idle_, (dword)idleThread);
    g_console->printf("thread=%x", (Thread*)thread);
    join(idle_, thread);
}

ProcessManager::~ProcessManager() {

    /* destroy */
    delete dispatchList_;
    delete idle_;
}

int ProcessManager::join(Process* process, Thread* thread) {

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

int ProcessManager::kill(Process* process) {

    /* remove from list */
    dispatchList_->remove(process);
    waitList_->remove(process);

    /* delete process */
    delete process;

    return NORMAL;
}

int ProcessManager::switchProcess() {

    /* do nothing ??? */

    return NORMAL;
}

LinearAddress ProcessManager::allocateKernelStack() const {
    static int i = 0;
    return 0x100000 + i * 4096;
}

bool ProcessManager::schedule() {

    bool      isProcessChanged;
    Process* next;
    Process* tmp;

    /* tick */
    current_->tick();

    /* process has time yet */
    if (current_->hasTimeLeft()) {

        /* next is current */
        return false;
    }

    /* round robin */
    current_->setTimeLeft(4);
    tmp = dispatchList_->removeAt(0);
    dispatchList_->add(tmp);
    next = dispatchList_->get(0);
    isProcessChanged = next != current_;
    current_ = next;
    return isProcessChanged;
}

Process* ProcessManager::create(int type, const char* name) {

    Process* result;

    /* process type */
    switch (type) {

      case USER_PROCESS:
          result = new UserProcess(name, pageManager_->createNewPageDirectory());
          break;
      case KERNEL_PROCESS:
          result = new KernelProcess(name, pageManager_->createNewPageDirectory());
          break;
      default:
          result = (Process*)NULL;
          break;
    }

    return result;
}

int ProcessManager::add(Process* process) {

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

Thread* ProcessManager::createThread(Process* process, dword programCounter) {

    /* check process */
    if (!(dispatchList_->hasElement(process)) && !(waitList_->hasElement(process))) {
        return (Thread*)NULL;
    }

    /* create thread */
    return process->createThread(programCounter);
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
Process::Process(const char* name, PageEntry* directory) : tick_(0), timeLeft_(4) {

    /* name */
    strncpy(name_, name, sizeof(name_));

    /* address space */
    pageDirectory_ = directory;

    g_page_manager->allocatePhysicalPage(directory, 0xFFFFFFFF, true, true, true);
}

Process::~Process() {
}

int Process::join(Thread* thread) {
    return threadManager_->join(thread);
}

Thread* Process::createThread(dword programCounter) {
    return threadManager_->create(programCounter, pageDirectory_);
}

Thread* Process::schedule() {
    return threadManager_->schedule();
}

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
UserProcess::UserProcess(const char* name, PageEntry* directory) : Process(name, directory) {

    /* not kernel mode */
    isUserMode_ = true;

    /* thread manager */
    threadManager_ = new ThreadManager(isUserMode_);
}

UserProcess::~UserProcess() {
}

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
KernelProcess::KernelProcess(const char* name, PageEntry* directory) : Process(name, directory) {

    /* kernel mode */
    isUserMode_ = false;

    /* thread manager */
    threadManager_ = new ThreadManager(isUserMode_);
}

KernelProcess::~KernelProcess() {
}

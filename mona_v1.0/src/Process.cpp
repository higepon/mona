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
#include <userlib.h>
#include <PageManager.h>

/*----------------------------------------------------------------------
    schedule
----------------------------------------------------------------------*/
void schedule() {

    /* Process schedule */
    bool isProcessChanged = g_processManager->schedule();

    /* Thread schedule */
    Process* current = g_processManager->getCurrentProcess();
    bool isUser = current->isUserMode();
    g_currentThread = current->schedule()->getThreadInfo();

    /* debug information */
//     ArchThreadInfo* i = g_currentThread->archinfo;
//     const char* name = g_processManager->getCurrentProcess()->getName();
//     g_console->printf("%s eip=%x esp=%x:%x,%x cr3=%x cs=%x\n", name, i->eip, i->ss, i->esp, i->ebp, i->cr3, i->cs);

    /* Thread switch */
    current->switchThread(isProcessChanged, isUser);
}

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
Thread::Thread() : tick_(0), timeLeft_(1) {

    /* thread information */
    threadInfo_ = new ThreadInfo;
    checkMemoryAllocate(threadInfo_, "class Thread info allocate");
    threadInfo_->thread = this;

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

    /* user or kernel or V86 */
    isUser_ = isUser;
    isV86_  = false;

    /* list of thread */
    dispatchList_ = new HList<Thread*>();
    waitList_     = new HList<Thread*>();
    checkMemoryAllocate(dispatchList_, "ThreadManager memory allocate list");
}

ThreadManager::ThreadManager(bool isUser, bool isV86) {

    /* user or kernel or V86 */
    isUser_ = isUser;
    isV86_  = isV86;

    /* list of thread */
    dispatchList_ = new HList<Thread*>();
    waitList_     = new HList<Thread*>();
    checkMemoryAllocate(dispatchList_, "ThreadManager memory allocate list");
}

ThreadManager::~ThreadManager() {

    for (int i = 0; i < dispatchList_->size(); i++) {
        kill(dispatchList_->get(i));
    }
    for (int i = 0; i < dispatchList_->size(); i++) {
        kill(waitList_->get(i));
    }

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
    if (isV86_) {
        archCreateV86Thread(thread, programCounter, pageDirectory);
    } else  if (isUser_) {
        archCreateUserThread(thread, programCounter, pageDirectory);
    } else {
        archCreateThread(thread, programCounter, pageDirectory);
    }
    return thread;
}

void ThreadManager::archCreateUserThread(Thread* thread, dword programCounter, PageEntry* pageDirectory) const {

    dword stackAddress = allocateStack();
    g_page_manager->allocatePhysicalPage(pageDirectory, stackAddress, true, true, true);

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
    ainfo->esp     = stackAddress;
    ainfo->ebp     = stackAddress;
    ainfo->esp0    = g_processManager->allocateKernelStack();
    ainfo->eip     = programCounter;
    ainfo->cr3     = (PhysicalAddress)pageDirectory;
}

void ThreadManager::archCreateThread(Thread* thread, dword programCounter, PageEntry* pageDirectory) const {

    dword stackAddress = allocateStack();
    g_page_manager->allocatePhysicalPage(pageDirectory, stackAddress, true, true, true);

    ThreadInfo* info      = thread->getThreadInfo();
    ArchThreadInfo* ainfo = info->archinfo;
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
    ainfo->esp     = stackAddress;
    ainfo->ebp     = stackAddress;
    ainfo->eip     = programCounter;
    ainfo->cr3     = (PhysicalAddress)pageDirectory;
}

void ThreadManager::archCreateV86Thread(Thread* thread, dword programCounter, PageEntry* pageDirectory) const {

    //    dword stackAddress = allocateStack();
    dword stackAddress = 0x200;
    g_page_manager->allocatePhysicalPage(pageDirectory, stackAddress, true, true, true);

    ThreadInfo* info      = thread->getThreadInfo();
    ArchThreadInfo* ainfo = info->archinfo;
//     ainfo->cs      = USER_CS;
//     ainfo->ds      = USER_DS;
//     ainfo->es      = USER_DS;
//     ainfo->ss      = USER_SS;
    ainfo->cs      = 0;
    ainfo->ds      = 0;
    ainfo->es      = 0;
    ainfo->ss      = 0;
    ainfo->ss0     = KERNEL_SS;
    ainfo->eflags  = 0x20200;
    ainfo->eax     = 0;
    ainfo->ecx     = 0;
    ainfo->edx     = 0;
    ainfo->ebx     = 0;
    ainfo->esi     = 0;
    ainfo->edi     = 0;
    ainfo->dpl     = DPL_USER;
    ainfo->esp     = stackAddress;
    ainfo->ebp     = stackAddress;
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
    delete thread;
    return NORMAL;
}


int ThreadManager::kill(List<Thread*>* list) {

    for (int i = list->size() - 1; i >=0; i--) {
        Thread* thread = list->get(i);
        list->removeAt(i);
        delete thread;
    }
    return NORMAL;
}

int ThreadManager::killAllThread() {

    kill(dispatchList_);
    kill(waitList_);
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

void ThreadManager::printAllThread() {

    Thread* thread;
    ArchThreadInfo* a;

    for (int i = 0; i < dispatchList_->size(); i++) {
        thread = dispatchList_->get(i);
        a = thread->getThreadInfo()->archinfo;
        g_console->printf("    %x %x %x %x %d                       \n", a->cr3, a->eip, a->esp, a->cs, thread->getTick());
    }

    for (int i = 0; i < waitList_->size(); i++) {
        thread = waitList_->get(i);
        a = thread->getThreadInfo()->archinfo;
        g_console->printf("    %x %x %x %x %d                       \n", a->cr3, a->eip, a->esp, a->cs, thread->getTick());
    }
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
const int ProcessManager::USER_PROCESS;
const int ProcessManager::KERNEL_PROCESS;
ProcessManager::ProcessManager(PageManager* pageManager) : tick_(0) {

    /* page manager */
    pageManager_ = pageManager;

    /* dispach and wait list */
    dispatchList_ = new HList<Process*>();
    waitList_     = new HList<Process*>();
    checkMemoryAllocate(dispatchList_, "ProcessManager dispatch list");
    checkMemoryAllocate(waitList_    , "ProcessManager wait     list");

    /* create idle process */
    PageEntry* directory = pageManager_->createNewPageDirectory();
    pageManager_->setPageDirectory((dword)directory);
    idle_ = new KernelProcess("Idle", directory);
    checkMemoryAllocate(idle_, "ProcessManager idle memory allcate");
    add(idle_);
    current_ = idle_;

    /* create thread for idle process */
    Thread* thread = createThread(idle_, (dword)idleThread);
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

    current_ = dispatchList_->get(0);

    /* delete process */
    delete process;
    return NORMAL;
}

void ProcessManager::killSelf() {
    asm volatile("movl $%c0, %%ebx \n"
                 "int  $0x80       \n"
                 :
                 :"g"(SYSTEM_CALL_KILL)
                 :"ebx"
                 );
}

int ProcessManager::switchProcess() {

    /* do nothing ??? */

    return NORMAL;
}

LinearAddress ProcessManager::allocateKernelStack() const {
    static int i = 0;
    i++;
    return 0x100000 + i * 4096;
}

bool ProcessManager::schedule() {

    bool      isProcessChanged;
    Process* next;
    Process* tmp;

    /* tick */
    this->tick();
    current_->tick();

    /* wakeup */
    wakeup();

    /* process has time yet */
    if (current_->hasTimeLeft()) {

        /* next is current */
        return false;
    }

    /* round robin */
    current_->setTimeLeft(5);
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
      case V86_PROCESS:
          result = new V86Process(name, pageManager_->createNewPageDirectoryForV86());
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

void ProcessManager::printProcess() {

    Process* p;

    for (int i = 0; i < dispatchList_->size(); i++) {
        p = dispatchList_->get(i);
        g_console->printf("[%s]dispatch %d                                           \n", p->getName(), p->getTick());
        p->printAllThread();
    }

    for (int i = 0; i < waitList_->size(); i++) {
        p = waitList_->get(i);
        g_console->printf("[%s]waiting %d                                             \n", p->getName(), p->getTick());
        p->printAllThread();
    }
}

Process* ProcessManager::find(const char* name) {

    if (!name) return (Process*)NULL;

    for (int i = 0; i < dispatchList_->size(); i++) {

        Process* process = dispatchList_->get(i);
        if (!strcmp(name, process->getName())) {
            return process;
        }
    }
    for (int i = 0; i < waitList_->size(); i++) {

        Process* process = waitList_->get(i);
        if (!strcmp(name, process->getName())) {
            return process;
        }
    }
    return (Process*)NULL;
}

Process* ProcessManager::find(dword pid) {

    for (int i = 0; i < dispatchList_->size(); i++) {

        Process* process = dispatchList_->get(i);
        if (pid == process->getPid()) {
            return process;
        }
    }
    for (int i = 0; i < waitList_->size(); i++) {

        Process* process = waitList_->get(i);
        if (pid == process->getPid()) {
            return process;
        }
    }
    return (Process*)NULL;
}

dword ProcessManager::lookup(const char* name) {

    if (!name) return 0;

    for (int i = 0; i < dispatchList_->size(); i++) {

        Process* process = dispatchList_->get(i);
        if (!strcmp(name, process->getName())) {
            return process->getPid();
        }
    }
    for (int i = 0; i < waitList_->size(); i++) {

        Process* process = waitList_->get(i);
        if (!strcmp(name, process->getName())) {
            return process->getPid();
        }
    }
    return 0;
}


int ProcessManager::sleep(Process* process, dword tick) {

    if (!dispatchList_->hasElement(process)) {
        return -1;
    }

    process->setWakeupTimer(getTick() + tick);
    dispatchList_->remove(process);
    waitList_->add(process);
    return NORMAL;
}

void ProcessManager::wakeup() {

    dword tick = getTick();

    for (int i = waitList_->size() - 1; i >=0; i--) {
        Process* process = waitList_->get(i);
        if (process->getWakeupTimer() < tick) {

            g_console->printf("%d=%s", i, process->getName());
            dispatchList_->add(process);
            waitList_->remove(process);
        }
    }
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
dword Process::pid = 0;
Process::Process(const char* name, PageEntry* directory) : tick_(0), wakeupTimer_(0xFFFFFFFF), timeLeft_(4) {

    /* name */
    strncpy(name_, name, sizeof(name_));

    /* address space */
    pageDirectory_ = directory;

    /* allocate heap */
    heap_ = new HeapSegment(0xC0000000, 8 * 1024 * 1024);

    /* shared list */
    shared_ = new HList<SharedMemorySegment*>();

    /* message list */
    messageList_ = new HList<MessageInfo*>();

    /* mutex tree */
    kmutexTree_ = new BinaryTree<KMutex*>();

    /* pid */
    pid++;
    pid_ = pid;
}

Process::~Process() {

    /* heap */
    delete heap_;

    /* shared MemorySegment */
    for (int i = 0; i < shared_->size(); i++) {
        SharedMemoryObject::detach(shared_->get(i)->getId(), this);
    }

    delete(shared_);

    /* message list */
    for (int i = 0; i < messageList_->size(); i++) {
        delete messageList_->get(i);
    }

    /* we need for each ! don't forge */
    delete kmutexTree_;
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
   delete threadManager_;
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
    delete threadManager_;
}

/*----------------------------------------------------------------------
    V86Process
----------------------------------------------------------------------*/
V86Process::V86Process(const char* name, PageEntry* directory) : Process(name, directory) {

    /* kernel mode */
    isUserMode_ = true;

    /* thread manager */
    threadManager_ = new ThreadManager(isUserMode_, true);
}

V86Process::~V86Process() {
    delete threadManager_;
}

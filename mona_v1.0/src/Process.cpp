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

#define FOREACH_Q(top, type, element) \
for (type element = (type )((top).next); element != &(top); element = (type )((element)->next))

#define PTR_THREAD(queue) (((Thread*)(queue))->getThreadInfo())
#define IN_SAME_SPACE(a, b) ((a->archinfo->cr3) == (b->archinfo->cr3))

typedef struct
{
    dword reason;
    dword who;
} WaitFor;

#define W_CRITICAL 0
#define W_HUMAN    1
#define W_NORMAL   2
#define WAITQ_IDX(waitReason) ((int)(waitReason & 0x03))

Scheduler::Scheduler() : runq(64), waitq(3)
{
    this->monaMin = this->runq.getLength() - 1;

    for (int i = 0; i < this->runq.getLength(); i++)
    {
        Queue::initialize(&(runq[i]));
    }

    for (int i = 0; i < this->waitq.getLength(); i++)
    {
        Queue::initialize(&(waitq[i]));
    }
}

Scheduler::~Scheduler()
{
}

int Scheduler::calcPriority(Thread* thread)
{
    thread->partTick /= 2;
    thread->currPriority = thread->basePriotity + thread->partTick;

    if (thread->currPriority > this->monaMin)
    {
        thread->currPriority = this->monaMin;
    }

    /* schedule done */
    thread->scheduled = this->tickTotal;

    return NORMAL;
}

bool Scheduler::schedule()
{
    /* schedule for each run queue */
    FOREACH(Thread, queue, runq)
    {
//        FOREACH_Q(queue, Thread*, thread)
	for (Thread* thread = (Thread*)(queue.next); thread != &queue; thread = (Thread*)(thread->next))
        {
            g_console->printf("[%x][1]", thread);
            /* already scheduled ? */
            if (this->tickTotal == thread->scheduled)
            {
            g_console->printf("[%x][2]", thread);
                continue;
            }

            g_console->printf("[%x][3]", thread);
            calcPriority(thread);

            g_console->printf("[%x][4]", thread);
            /* insert into runq[priority] */
            Thread* prev = (Thread*)(thread->prev);
            g_console->printf("[%x][5]", thread);
            Queue::remove(thread);
            g_console->printf("[%x][6]", thread);
            Thread targetQueue = runq[thread->currPriority];
            g_console->printf("[%x][7]", thread);
            Queue::addToPrev(&targetQueue, thread);
            thread = prev;
        }
    }

    for (;;);

    FOREACH(Thread, queue, runq)
    {
        if (Queue::isEmpty(&queue))
        {
            continue;
        }
        else
        {
            g_prevThread    = g_currentThread;
            g_currentThread = PTR_THREAD(Queue::top(&queue));
            break;
        }
    }

    return IN_SAME_SPACE(g_prevThread, g_currentThread);
}

void Scheduler::join(Thread* thread, int priority)
{
    if (priority > monaMin || priority < 0) return;

    Thread targetQueue = runq[0];
    Queue::addToPrev(&targetQueue, thread);

    this->schedule();
}

int Scheduler::kill(Thread* thread)
{
    Queue::remove(thread);
    this->schedule();
    return NORMAL;
}

int Scheduler::wait(Thread* thread, int waitReason)
{
    thread->setWaitReason(waitReason);

    Queue::remove(thread);
    Thread targetQueue = waitq[WAITQ_IDX(waitReason)];
    Queue::addToPrev(&targetQueue, thread);
    return NORMAL;
}

int Scheduler::wakeup(Thread* thread, int waitReason)
{
    if (thread->getWaitReason() != waitReason)
    {
        return -1;
    }

    Queue::remove(thread);
    Thread targetQueue = runq[0];
    Queue::addToPrev(&targetQueue, thread);

    this->schedule();
    return NORMAL;
}

/*----------------------------------------------------------------------
    Queue
----------------------------------------------------------------------*/
void Queue::initialize(Queue* queue)
{
    queue->prev = queue;
    queue->next = queue;
}

void Queue::addToNext(Queue* p, Queue* q)
{
    q->next = p->next;
    q->prev = p;
    p->next->prev = q;
    p->next = q;
}

void Queue::addToPrev(Queue* p, Queue* q)
{
    q->prev = p->prev;
    q->next = p;
    p->prev->next = q;
    p->prev = q;
}

void Queue::remove(Queue* p)
{
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

bool Queue::isEmpty(Queue* p)
{
    return (p->next == p);
}

Queue* Queue::removeNext(Queue* p)
{
    Queue* result = p->next;
    p->next = result->next;
    result->next->prev = p;
    return result;
}

Queue* Queue::top(Queue* root)
{
    return root->next;
}

/*----------------------------------------------------------------------
    ProcessOperation
----------------------------------------------------------------------*/
PageManager* ProcessOperation::pageManager;
const int ProcessOperation::USER_PROCESS;
const int ProcessOperation::KERNEL_PROCESS;

void ProcessOperation::initialize(PageManager* manager)
{
    ProcessOperation::pageManager = manager;
}

LinearAddress ProcessOperation::allocateKernelStack()
{
    static int i = 0;
    i++;
    return KERNEL_STACK_START + i * KERNEL_STACK_UNIT_SIZE;
}

Process* ProcessOperation::create(int type, const char* name)
{
    Process* result;

    switch (type)
    {
      case USER_PROCESS:
          result = new UserProcess(name, ProcessOperation::pageManager->createNewPageDirectory());
          break;
      case KERNEL_PROCESS:
          result = new KernelProcess(name, ProcessOperation::pageManager->createNewPageDirectory());
          break;
      default:
          result = (Process*)NULL;
          break;
    }
    return result;
}

Thread* ThreadOperation::create(Process* process, dword programCounter)
{
    Thread* thread = new Thread();
    PageEntry* directory = process->getPageDirectory();
    LinearAddress stack  = process->allocateStack();

    thread->getThreadInfo()->process = process;

    if (process->isUserMode())
    {
        archCreateUserThread(thread, programCounter, directory, stack);
    }
    else
    {
        archCreateThread(thread, programCounter, directory, stack);
    }

    (process->threadNum)++;
    return thread;
};

void ThreadOperation::archCreateUserThread(Thread* thread, dword programCounter
                                           , PageEntry* pageDirectory, LinearAddress stack)
{
    ProcessOperation::pageManager->allocatePhysicalPage(pageDirectory, stack, true, true, true);

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
    ainfo->esp     = stack;
    ainfo->ebp     = stack;
    ainfo->esp0    = ProcessOperation::allocateKernelStack();
    ainfo->eip     = programCounter;
    ainfo->cr3     = (PhysicalAddress)pageDirectory;
}

void ThreadOperation::archCreateThread(Thread* thread, dword programCounter
                                       , PageEntry* pageDirectory, LinearAddress stack)
{
    ProcessOperation::pageManager->allocatePhysicalPage(pageDirectory, stack, true, true, true);

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
    ainfo->esp     = stack;
    ainfo->ebp     = stack;
    ainfo->eip     = programCounter;
    ainfo->cr3     = (PhysicalAddress)pageDirectory;
}

int ThreadOperation::switchThread(bool isProcessChanged)
{
    bool isUser = g_currentThread->process->isUserMode() && (g_currentThread->archinfo->cs & 0x03);

    if (isProcessChanged && isUser)
    {
        /* address space & therad switch */
        arch_switch_thread_to_user2();
    }
    else if (!isProcessChanged && isUser)
    {
        /* only thread switch */
        arch_switch_thread_to_user1();
    }
    else if (isProcessChanged && !isUser)
    {
        /* address space & therad switch */
        arch_switch_thread2();
    }
    else
    {
        arch_switch_thread1();
    }

    /* does not come here */
    return NORMAL;
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
    delete threadInfo_->archinfo;
    delete threadInfo_;
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
dword Process::pid = 0;
Process::Process(const char* name, PageEntry* directory) : threadNum(0), tick_(0), wakeupTimer_(0xFFFFFFFF), timeLeft_(4) {

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

    /* argument list */
    arguments_ = new HList<char*>();

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

    /* arguments */
    for (int i = 0; i < arguments_->size(); i++) {
        delete[](arguments_->get(i));
    }

    delete arguments_;
}

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
UserProcess::UserProcess(const char* name, PageEntry* directory) : Process(name, directory)
{
    /* not kernel mode */
    isUserMode_ = true;
}

UserProcess::~UserProcess()
{
}

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
KernelProcess::KernelProcess(const char* name, PageEntry* directory) : Process(name, directory)
{
    /* kernel mode */
    isUserMode_ = false;
}

KernelProcess::~KernelProcess()
{
}

/*----------------------------------------------------------------------
    V86Process
----------------------------------------------------------------------*/
V86Process::V86Process(const char* name, PageEntry* directory) : Process(name, directory)
{
    /* kernel mode */
    isUserMode_ = true;
}

V86Process::~V86Process()
{
}

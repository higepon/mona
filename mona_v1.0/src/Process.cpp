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

#define FOREACH_N(top, type, element) \
for (type element = (type )((top)->next); element != (top); element = (type )((element)->next))

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

Scheduler::Scheduler() : runq(64), waitq(3), tickTotal(0)
{
    /* initialize run queue */
    for (int i = 0; i < runq.getLength(); i++)
    {
        runq[i] = new Thread();
        runq[i]->initialize();
    }

    /* initialize wait queue */
    for (int i = 0; i < waitq.getLength(); i++)
    {
        waitq[i] = new Thread();
        waitq[i]->initialize();
    }

    this->monaMin = this->runq.getLength() - 1;
}

Scheduler::~Scheduler()
{
}

void Scheduler::tick()
{
    (this->tickTotal)++;
}

dword Scheduler::getTick() const
{
    return tickTotal;
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

bool Scheduler::schedule2()
{
    Thread* root = runq[g_currentThread->thread->currPriority];
    Thread* curr = (Thread*)(root->top());

    curr->remove();
    root->addToPrev(curr);

    g_prevThread    = g_currentThread;
    g_currentThread = PTR_THREAD(root->top());
    return !(IN_SAME_SPACE(g_prevThread, g_currentThread));
}

bool Scheduler::schedule()
{
    /* schedule for each run queue */
    FOREACH(Thread*, queue, runq)
    {

        FOREACH_N(queue, Thread*, thread)
        {
            /* already scheduled ? */
            if (this->tickTotal == thread->scheduled)
            {
                continue;
            }

            calcPriority(thread);

            /* insert into runq[priority] */
            Thread* prev = (Thread*)(thread->prev);
            thread->remove();
            runq[thread->currPriority]->addToPrev(thread);
            thread = prev;
        }
    }

    FOREACH(Thread*, queue, runq)
    {
        if (queue->isEmpty())
        {
            continue;
        }
        else
        {
            g_prevThread    = g_currentThread;
            g_currentThread = PTR_THREAD(queue->top());
            break;
        }
    }

#if 0
    g_console->printf("[%s]\n", g_currentThread->process->getName());
    ArchThreadInfo* i = g_currentThread->archinfo;
    g_console->printf("\n");
    g_console->printf("eax=%x ebx=%x ecx=%x edx=%x\n", i->eax, i->ebx, i->ecx, i->edx);
    g_console->printf("esp=%x ebp=%x esi=%x edi=%x\n", i->esp, i->ebp, i->esi, i->edi);
    g_console->printf("cs =%x ds =%x ss =%x cr3=%x\n", i->cs , i->ds , i->ss , i->cr3);
    g_console->printf("eflags=%x eip=%x\n", i->eflags, i->eip);
#endif
    return !(IN_SAME_SPACE(g_prevThread, g_currentThread));
}

void Scheduler::join(Thread* thread, int priority)
{
    if (priority > monaMin || priority < 0) return;

    runq[0]->addToPrev(thread);

    this->schedule();
}

int Scheduler::kill(Thread* thread)
{
    thread->remove();
    this->schedule();
    return NORMAL;
}

int Scheduler::wait(Thread* thread, int waitReason)
{
    thread->setWaitReason(waitReason);

    thread->remove();
    waitq[WAITQ_IDX(waitReason)]->addToPrev(thread);
    return NORMAL;
}

int Scheduler::wakeup(Thread* thread, int waitReason)
{
    if (thread->getWaitReason() != waitReason)
    {
        return -1;
    }

    thread->remove();
    runq[0]->addToPrev(thread);

    this->schedule();
    return NORMAL;
}

/*----------------------------------------------------------------------
    Node
----------------------------------------------------------------------*/
void Node::initialize()
{
    this->prev = this;
    this->next = this;
}

void Node::addToNext(Node* q)
{
    q->next = this->next;
    q->prev = this;
    this->next->prev = q;
    this->next = q;
}

void Node::addToPrev(Node* q)
{
    q->prev = this->prev;
    q->next = this;
    this->prev->next = q;
    this->prev = q;
}

void Node::remove()
{
    this->prev->next = this->next;
    this->next->prev = this->prev;
}

bool Node::isEmpty()
{
    return (this->next == this);
}

Node* Node::removeNext()
{
    Node* result = this->next;
    this->next = result->next;
    result->next->prev = this;
    return result;
}

Node* Node::top()
{
    return this->next;
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
    (process->threadNum)++;
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

/*----------------------------------------------------------------------
    Idle function
----------------------------------------------------------------------*/
void monaIdle()
{
    for (;;)
    {
        arch_idle();
    }
}

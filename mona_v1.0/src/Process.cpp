/*!
    \file  Process.cpp
    \brief class Process

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/

#include <global.h>
#include <Process.h>
#include <PageManager.h>
#include <string.h>

#define FOREACH_N(top, type, element) \
for (type element = (type )((top)->next); element != (top); element = (type )((element)->next))

#define PTR_THREAD(queue) (((Thread*)(queue))->tinfo)
#define IN_SAME_SPACE(a, b) ((a->archinfo->cr3) == (b->archinfo->cr3))

#define W_CRITICAL 0
#define W_HUMAN    1
#define W_NORMAL   2
#define WAITQ_IDX(waitReason) ((int)(waitReason & 0x03))


Scheduler::Scheduler() : tickTotal(0)
{
    runq  = new Thread();
    waitq = new Thread();

    runq->initialize();
    waitq->initialize();
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

int Scheduler::sleep(Thread* thread, dword tick)
{
    dword now = this->getTick();
    thread->wakeupTimer = now + tick;
    return wait(thread, WAIT_TIMER);
}

Process* Scheduler::findProcess(dword pid)
{
    FOREACH_N(runq, Thread*, thread)
    {
        Process* process = thread->tinfo->process;

        if (process->getPid() == pid)
        {
            return process;
        }
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        Process* process = thread->tinfo->process;
        if (process->getPid() == pid)
        {
            return process;
        }
    }
    return (Process*)NULL;
}

Process* Scheduler::findProcess(const char* name)
{
    FOREACH_N(runq, Thread*, thread)
    {
        Process* process = thread->tinfo->process;
        if (!strcmp(name, process->getName()))
        {
            return process;
        }
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        Process* process = thread->tinfo->process;
        if (!strcmp(name, process->getName()))
        {
            return process;
        }
    }

    return (Process*)NULL;
}

dword Scheduler::lookup(const char* name)
{
    Process* process = findProcess(name);

    if (process == (Process*)NULL)
    {
        return 0xFFFFFFFF;
    }
    else
    {
        return process->getPid();
    }
}

bool Scheduler::schedule()
{
    Thread* current = (Thread*)(runq->top());
    current->remove();
    runq->addToPrev(current);

    wakeupTimer();

    g_prevThread    = g_currentThread;
    g_currentThread = PTR_THREAD(runq->top());

    return !(IN_SAME_SPACE(g_prevThread, g_currentThread));
}

int Scheduler::wakeupTimer()
{
    FOREACH_N(waitq, Thread*, thread)
    {
        if (thread->waitReason != WAIT_TIMER)
        {
            continue;
        }

        if (thread->wakeupTimer > getTick())
        {
            continue;
        }

        Thread* target = thread;
        thread = (Thread*)(thread->prev);

        target->remove();
        target->waitReason = WAIT_NONE;
        runq->addToNext(target);
    }
    return 0;
}

bool Scheduler::setCurrentThread()
{
    g_prevThread    = g_currentThread;
    g_currentThread = PTR_THREAD(runq->top());
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

void Scheduler::join(Thread* thread)
{
    runq->addToPrev(thread);
}

int Scheduler::kill(Thread* thread)
{
    thread->remove();
    return NORMAL;
}

int Scheduler::wait(Thread* thread, int waitReason)
{
    thread->waitReason = waitReason;

    thread->remove();
    waitq->addToPrev(thread);
    return NORMAL;
}

int Scheduler::wakeup(Thread* thread, int waitReason)
{
    if (thread->waitReason != waitReason)
    {
        return 0;
    }

    thread->remove();
    runq->addToNext(thread);
    thread->waitReason = WAIT_NONE;

    return this->setCurrentThread() ? 1 : -1;
}

int Scheduler::wakeup(Process* process, int waitReason)
{
    int count  = 0;
    int wakeupResult;
    bool processChanged = false;

    List<Thread*>* list = process->getThreadList();

    for (int i = 0; i < list->size(); i++)
    {
        wakeupResult = wakeup(list->get(i), waitReason);

        if (wakeupResult != 0)
        {
            processChanged = processChanged || (wakeupResult == 1);
            count++;
        }
    }

    if (count == 0)
    {
        return 0;
    }
    else
    {
        return (processChanged ? 1 : -1);
    }
}

void Scheduler::dump()
{
    g_console->printf("current=%x, prev=%x\n", g_currentThread, g_prevThread);
    FOREACH_N(runq, Thread*, thread)
    {
        ThreadInfo* i = PTR_THREAD(thread);
        g_console->printf("[r][%s,th=%x,eip=%x,cr3=%x esp0=%x\n", i->process->getName(), thread, i->archinfo->eip, i->archinfo->cr3, i->archinfo->esp0);
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        ThreadInfo* i = PTR_THREAD(thread);
        g_console->printf("[w][%s,th=%x,eip=%x,cr3=%x esp0=%x\n", i->process->getName(), thread, i->archinfo->eip, i->archinfo->cr3, i->archinfo->esp0);
    }
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

    thread->tinfo->process = process;

    if (process->isUserMode())
    {
        archCreateUserThread(thread, programCounter, directory, stack);
    }
    else
    {
        archCreateThread(thread, programCounter, directory, stack);
    }

    process->getThreadList()->add(thread);
    return thread;
};

void ThreadOperation::archCreateUserThread(Thread* thread, dword programCounter
                                           , PageEntry* pageDirectory, LinearAddress stack)
{
    ProcessOperation::pageManager->allocatePhysicalPage(pageDirectory, stack, true, true, true);

    ThreadInfo* info      = thread->tinfo;
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

    ThreadInfo* info      = thread->tinfo;
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

int ThreadOperation::switchThread(bool isProcessChanged, int num)
{
    bool isUser = g_currentThread->process->isUserMode() && (g_currentThread->archinfo->cs & 0x03);

#if 0
    ArchThreadInfo* i = g_currentThread->archinfo;
    logprintf("[%d]esp=%x ebp=%x cs =%d ds =%d ss =%d cr3=%x eflags=%x eip=%x ss0=%d esp0=%x gss0=%d gesp0=%x %s %s\n", num, i->esp, i->ebp, i->cs, i->ds, i->ss, i->cr3, i->eflags, i->eip, i->ss0, i->esp0, g_tss->ss0, g_tss->esp0,  g_currentThread->process->getName(), g_prevThread->process ? g_prevThread->process->getName() : "");

#endif

#if 0
    ArchThreadInfo* i = g_currentThread->archinfo;
    int x, y;
    g_console->getCursor(&x, &y);
    g_console->setCursor(3, 30);
    g_console->printf("(cs=%x, esp=%x, eflags=%x, eip=%x)", i->cs, i->esp, i->eflags, i->eip);
    g_console->setCursor(x, y);
#endif

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

int ThreadOperation::kill()
{
    Thread* thread   = g_currentThread->thread;
    Process* process = thread->tinfo->process;

    g_scheduler->kill(thread);
    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        delete process;
    }

    bool isProcessChange = g_scheduler->schedule();
    delete thread;
    ThreadOperation::switchThread(isProcessChange, 5);
    return NORMAL;
}

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
Thread::Thread() : waitReason(WAIT_NONE), totalTick(0)
{
    /* thread information */
    tinfo = new ThreadInfo;
    checkMemoryAllocate(tinfo, "class Thread info allocate");
    tinfo->thread = this;

    /* thread information arch dependent */
    tinfo->archinfo = new ArchThreadInfo;
    checkMemoryAllocate(tinfo->archinfo, "class Thread arch info allocate");
}

Thread::~Thread()
{
    /* free memory */
    delete tinfo->archinfo;
    delete tinfo;
}

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
dword Process::pid = 0;
Process::Process(const char* name, PageEntry* directory) : threadNum(0)
{
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

    threadList_ = new HList<Thread*>();

    /* pid */
    pid++;
    pid_ = pid;
}

Process::~Process()
{
    /* heap */
    delete heap_;

    /* shared MemorySegment */
    for (int i = 0; i < shared_->size(); i++)
    {
        SharedMemoryObject::detach(shared_->get(i)->getId(), this);
    }

    delete(shared_);

    /* message list */
    for (int i = 0; i < messageList_->size(); i++)
    {
        delete messageList_->get(i);
    }

    /* we need for each ! don't forget */
    delete kmutexTree_;

    /* arguments */
    for (int i = 0; i < arguments_->size(); i++)
    {
        delete[](arguments_->get(i));
    }

    delete arguments_;
    delete threadList_;
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
#if 0
        static dword count = 0;
        if (count % 0xFFFFFFF)  g_console->printf(".");
        if (count % 20000000) g_scheduler->dump();
        count++;
#endif
        asm volatile("hlt");
//        arch_idle();
    }
}

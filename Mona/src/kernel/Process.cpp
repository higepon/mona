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

#include <sys/List.h>
#include <sys/HList.h>
#include "global.h"
#include "Process.h"
#include "PageManager.h"
#include "string.h"

#define FOREACH_N(top, type, element) \
for (type element = (type )((top)->next); element != (top); element = (type )((element)->next))

#define PTR_THREAD(queue) (((Thread*)(queue))->tinfo)
#define IN_SAME_SPACE(a, b) ((a->archinfo->cr3) == (b->archinfo->cr3))

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

dword Scheduler::lookupMainThread(const char* name)
{
    Process* process = findProcess(name);

    if (process == NULL)
    {
        return 0xFFFFFFFF;
    }

    List<Thread*>* list = process->getThreadList();
    dword found = 0xFFFFFFFF;

    for (int i = 0; i < list->size(); i++)
    {
        dword id = list->get(i)->id;

        if (id < found)
        {
            found = id;
        }
    }
    return found;
}

Thread* Scheduler::find(dword id)
{
    FOREACH_N(runq, Thread*, thread)
    {
        if (id == thread->id)
        {
            return thread;
        }
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        if (id == thread->id)
        {
            return thread;
        }
    }

    return (Thread*)NULL;
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

    wakeupEvents();

    g_prevThread    = g_currentThread;
    g_currentThread = PTR_THREAD(runq->top());

    return !(IN_SAME_SPACE(g_prevThread, g_currentThread));
}

int Scheduler::wakeupEvents()
{
    FOREACH_N(waitq, Thread*, thread)
    {
        if ((thread->waitReason == WAIT_TIMER && thread->wakeupTimer <= getTick()))
        {
            Thread* target = thread;
            thread = (Thread*)(thread->prev);

            target->remove();
            target->waitReason = WAIT_NONE;
            runq->addToNext(target);
        }
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
    List<Thread*>* list = thread->tinfo->process->getThreadList();
    list->add(thread);
    runq->addToPrev(thread);
}

int Scheduler::kill(Thread* thread)
{
    List<Thread*>* list = thread->tinfo->process->getThreadList();
    list->remove(thread);
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
    logprintf("current=%x, prev=%x\n", g_currentThread, g_prevThread);
    FOREACH_N(runq, Thread*, thread)
    {
        ThreadInfo* i = PTR_THREAD(thread);
//        g_console->printf("[r][%s,th=%x,eip=%x,cr3=%x esp0=%x\n", i->process->getName(), thread, i->archinfo->eip, i->archinfo->cr3, i->archinfo->esp0);
        logprintf("[r][%s,th=%x,eip=%x,cr3=%x esp0=%x\n", i->process->getName(), thread, i->archinfo->eip, i->archinfo->cr3, i->archinfo->esp0);
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        ThreadInfo* i = PTR_THREAD(thread);
        logprintf("[w][%s,th=%x,eip=%x,cr3=%x esp0=%x wreason=%x\n", i->process->getName(), thread, i->archinfo->eip, i->archinfo->cr3, i->archinfo->esp0, thread->waitReason);
    }
}

void Scheduler::setDump()
{
    g_ps.next = NULL;

    PsInfo* current = &g_ps;

    FOREACH_N(runq, Thread*, thread)
    {
        ThreadInfo* i = PTR_THREAD(thread);

        current->next = new PsInfo;
        current  = current->next;

        strncpy(current->name, i->process->getName(), sizeof(current->name));
        current->cr3   = i->archinfo->cr3;
        current->eip   = i->archinfo->eip;
        current->esp   = i->archinfo->esp;
        current->tid   = thread->id;
        current->state = 1;
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        ThreadInfo* i = PTR_THREAD(thread);

        current->next = new PsInfo;
        current  = current->next;

        strncpy(current->name, i->process->getName(), sizeof(current->name));
        current->cr3   = i->archinfo->cr3;
        current->eip   = i->archinfo->eip;
        current->esp   = i->archinfo->esp;
        current->tid   = thread->id;
        current->state = 0;
    }

    dumpCurrent = g_ps.next;
    current->next = NULL;
}

PsInfo* Scheduler::readDump()
{
    if (dumpCurrent == NULL)
    {
        return NULL;
    }

    PsInfo* info = dumpCurrent;
    dumpCurrent = dumpCurrent->next;
    return info;
}

dword* Scheduler::getAllThreadID(dword* threadNum)
{
    dword* result;
    dword i     = 0;
    dword count = 0;

    FOREACH_N(runq, Thread*, thread)  count++;
    FOREACH_N(waitq, Thread*, thread) count++;

    result = new dword[count];
    if (result == NULL) return NULL;

    FOREACH_N(runq, Thread*, thread)
    {
        result[i] = thread->id;
        i++;
    }

    FOREACH_N(waitq, Thread*, thread)
    {
        result[i] = thread->id;
        i++;
    }

    *threadNum = count;
    return result;
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

/*----------------------------------------------------------------------
    ThreadOperation
----------------------------------------------------------------------*/
dword ThreadOperation::id = 56;
Thread* ThreadOperation::create(Process* process, dword programCounter)
{

    logprintf("before idle2.1\n");
    Thread* thread = new Thread();
    logprintf("before idle2.2\n");
    (process->threadNum)++;
    PageEntry* directory = process->getPageDirectory();

    logprintf("before idle2.3\n");

    thread->tinfo->process = process;

    logprintf("before idle2.4\n");
    thread->id = ThreadOperation::id++;

    if (process->isUserMode())
    {
        LinearAddress stack  = process->allocateStack();
    logprintf("before idle2.5\n");
        archCreateUserThread(thread, programCounter, directory, stack);
    }
    else
    {
        LinearAddress stack  = ProcessOperation::allocateKernelStack();
    logprintf("before idle2.6\n");
        archCreateThread(thread, programCounter, directory, stack);
    logprintf("before idle2.7\n");
    }

    process->getThreadList()->add(thread);
    return thread;
};

void ThreadOperation::archCreateUserThread(Thread* thread, dword programCounter
                                           , PageEntry* pageDirectory, LinearAddress stack)
{
    ProcessOperation::pageManager->allocatePhysicalPage(pageDirectory, stack - Process::STACK_SIZE, true, true, true);

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

    /* fpu (=fninit) */
    ainfo->fpu[0] = 0xFFFF037F;
    ainfo->fpu[1] = 0xFFFF0000;
    ainfo->fpu[2] = 0xFFFFFFFF;
    ainfo->fpu[3] = 0x00000000;
    ainfo->fpu[4] = 0x00000000;
    ainfo->fpu[5] = 0x00000000;
    ainfo->fpu[6] = 0xFFFF0000;
}

void ThreadOperation::archCreateThread(Thread* thread, dword programCounter
                                       , PageEntry* pageDirectory, LinearAddress stack)
{
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

   /* fpu (=fninit) */
    ainfo->fpu[0] = 0xFFFF037F;
    ainfo->fpu[1] = 0xFFFF0000;
    ainfo->fpu[2] = 0xFFFFFFFF;
    ainfo->fpu[3] = 0x00000000;
    ainfo->fpu[4] = 0x00000000;
    ainfo->fpu[5] = 0x00000000;
    ainfo->fpu[6] = 0xFFFF0000;
}

int ThreadOperation::switchThread(bool isProcessChanged, int num)
{
    bool isUser = g_currentThread->process->isUserMode() && (g_currentThread->archinfo->cs & 0x03);

#if 0
    ArchThreadInfo* i = g_currentThread->archinfo;
    logprintf("[%d]esp=%x ebp=%x cs =%d ds =%d ss =%d cr3=%x eflags=%x eip=%x ss0=%d esp0=%x eax=%x gss0=%d gesp0=%x %s %s p(%s) u(%s)\n", num, i->esp, i->ebp, i->cs, i->ds, i->ss, i->cr3, i->eflags, i->eip, i->ss0, i->esp0, i->eax, g_tss->ss0, g_tss->esp0,  g_currentThread->process->getName(), g_prevThread->process ? g_prevThread->process->getName() : "", isProcessChanged ? "t" : "f", isUser ? "t": "f");
#endif


#if 0
    for (int j = 0; j < 27; j++)
    {
        logprintf("fpu[%d]=%x", j, i->fpu[j]);
    }

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

    sendKilledMessage();

    g_scheduler->kill(thread);
    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    bool isProcessChange = g_scheduler->schedule();
    delete thread;
    ThreadOperation::switchThread(isProcessChange, 5);
    return NORMAL;
}

int ThreadOperation::kill(dword tid)
{
    Thread* thread   = g_scheduler->find(tid);
    if (thread == NULL) return -1;

    Process* process = thread->tinfo->process;

    g_scheduler->kill(thread);
    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    bool isProcessChange = g_scheduler->schedule();
    delete thread;
    ThreadOperation::switchThread(isProcessChange, 5);
    return NORMAL;
}

void ThreadOperation::sendKilledMessage()
{
#if 0
    dword threadNum;
    dword* list;
    MessageInfo msg;

    list = g_scheduler->getAllThreadID(&threadNum);

    /* set message */
    msg.header = MSG_THREAD_KILLED;
    msg.arg1   = g_currentThread->thread->id;

    g_console->printf("here");

    if (list == NULL) return;

    g_console->printf("here2");

    for (dword i = 0; i < threadNum; i++)
    {
    g_console->printf("here3");
        dword id = list[i];
        if (id == msg.arg1) continue;
        dword result;
        SYSCALL_2(SYSTEM_CALL_SEND, result, id, (void*)(&msg));
    }
    g_console->printf("here4");
    delete[] list;
#endif
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

    messageList = new HList<MessageInfo*>();
    checkMemoryAllocate(tinfo->archinfo, "class Thread messageList allocate");
}

Thread::~Thread()
{
    /* free memory */
    delete messageList;
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

    /* arguments */
    for (int i = 0; i < arguments_->size(); i++)
    {
        delete[](arguments_->get(i));
    }

    delete messageList_;
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

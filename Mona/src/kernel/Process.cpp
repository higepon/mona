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


#define PTR_THREAD(queue) (((Thread*)(queue))->tinfo)

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
    Thread* thread = new Thread(30);

    (process->threadNum)++;
    PageEntry* directory = process->getPageDirectory();

    thread->tinfo->process = process;

    thread->id = ThreadOperation::id++;

    if (process->isUserMode())
    {
        LinearAddress stack  = process->allocateStack();
        archCreateUserThread(thread, programCounter, directory, stack);
    }
    else
    {
        LinearAddress stack  = ProcessOperation::allocateKernelStack();
        archCreateThread(thread, programCounter, directory, stack);
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
    logprintf("prev=%d curr=%d\n", g_prevThread->thread->id, g_currentThread->thread->id);
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

    g_scheduler->Kill(thread);
    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    bool isProcessChange = g_scheduler->Schedule3();
    delete thread;
    ThreadOperation::switchThread(isProcessChange, 5);
    return NORMAL;
}

int ThreadOperation::kill(dword tid)
{
    Thread* thread   = g_scheduler->Find(tid);
    if (thread == NULL) return -1;

    Process* process = thread->tinfo->process;

    g_scheduler->Kill(thread);
    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    bool isProcessChange = g_scheduler->Schedule3();
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
Thread::Thread() : waitEvent(MEvent::NONE), basePriority(0), lastCpuUsedTick(0), age(0)
{
    /* thread information */
    tinfo = new ThreadInfo;
    ASSERT(tinfo);
    tinfo->thread = this;

    /* thread information arch dependent */
    tinfo->archinfo = new ArchThreadInfo;
    ASSERT(tinfo->archinfo);

    messageList = new HList<MessageInfo*>();
    ASSERT(messageList);

    priority = basePriority;
}

Thread::Thread(dword basePriority) : waitEvent(MEvent::NONE), basePriority(basePriority), lastCpuUsedTick(0), age(0)
{
    /* thread information */
    tinfo = new ThreadInfo;
    ASSERT(tinfo);
    tinfo->thread = this;

    /* thread information arch dependent */
    tinfo->archinfo = new ArchThreadInfo;
    ASSERT(tinfo->archinfo);

    messageList = new HList<MessageInfo*>();
    ASSERT(messageList);

    priority = basePriority;
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

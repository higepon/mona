/*!
    \file  Process.cpp
    \brief class Process

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

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
#include "BitMap.h"

#define PTR_THREAD(queue) (((Thread*)(queue))->tinfo)

/*----------------------------------------------------------------------
    ProcessOperation
----------------------------------------------------------------------*/
PageManager* ProcessOperation::pageManager;
const int ProcessOperation::USER_PROCESS;
const int ProcessOperation::KERNEL_PROCESS;

static BitMap* kernelStackMap;

void ProcessOperation::initialize(PageManager* manager)
{
    ProcessOperation::pageManager = manager;

    kernelStackMap = new BitMap(KERNEL_STACK_SIZE / KERNEL_STACK_UNIT_SIZE);
}

LinearAddress ProcessOperation::allocateKernelStack()
{
    int stackIndex = kernelStackMap->find();

    if (stackIndex == BitMap::NOT_FOUND)
    {
        panic("sorry no kernel stack\n");
        for (;;);
    }

    return KERNEL_STACK_START + stackIndex * KERNEL_STACK_UNIT_SIZE;
}

void ProcessOperation::freeKernelStack(LinearAddress address)
{
    if (address < KERNEL_STACK_START || address > KERNEL_STACK_SIZE)
    {
        return;
    }

    int index = (address - KERNEL_STACK_START) / KERNEL_STACK_UNIT_SIZE;

    kernelStackMap->clear(index);
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
    Thread* thread = new Thread();

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
    /* stack size = 8K */
    ProcessOperation::pageManager->allocatePhysicalPage(pageDirectory, stack - Process::STACK_SIZE / 2, true, true, true);
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
    ainfo->esp0    = ProcessOperation::allocateKernelStack() + 0x1000;  //added by TAKA
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
    
    /* add by TAKA */
    thread->kernelStackBottom = ainfo->esp0;
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
    ainfo->esp     = stack + 0x1000;    //added by TAKA
    ainfo->ebp     = stack + 0x1000;    //added by TAKA
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
    
    /* add by TAKA */
    thread->kernelStackBottom = stack;
}

int ThreadOperation::switchThread(bool isProcessChanged, int num)
{
    bool isUser = g_currentThread->process->isUserMode() && (g_currentThread->archinfo->cs & 0x03);

#if 0
    ArchThreadInfo* i = g_currentThread->archinfo;
    logprintf("[%d]esp=%x ebp=%x cs =%d ds =%d ss =%d cr3=%x eflags=%x eip=%x ss0=%d esp0=%x eax=%x gss0=%d gesp0=%x %s(%d) %s(%d) p(%s) u(%s)\n", num, i->esp, i->ebp, i->cs, i->ds, i->ss, i->cr3, i->eflags, i->eip, i->ss0, i->esp0, i->eax, g_tss->ss0, g_tss->esp0,  g_currentThread->process->getName(), g_currentThread->thread->priority, g_prevThread->process ? g_prevThread->process->getName() : "", g_prevThread->thread->priority, isProcessChanged ? "t" : "f", isUser ? "t": "f");
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

//    logprintf("kill:%s", process->getName());

    g_scheduler->Kill(thread);

//    logprintf("%s:%d\n", __FILE__, __LINE__);

#if 0

    /* if do this, FileOutputStream hang up@hello.cpp*/
    sendKilledMessage();
#endif

//    logprintf("%s:%d\n", __FILE__, __LINE__);

    (process->threadNum)--;

//    logprintf("%s:%d\n", __FILE__, __LINE__);

    //modified by TAKA
    //ProcessOperation::freeKernelStack(process->getStackBottom(thread));
    ProcessOperation::freeKernelStack(thread->kernelStackBottom);

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

//    logprintf("%s:%d\n", __FILE__, __LINE__);

    delete thread;

//    logprintf("%s:%d\n", __FILE__, __LINE__);

    g_scheduler->SwitchToNext();

    /* not reached */

    return NORMAL;
}

int ThreadOperation::kill(dword tid)
{
    Thread* thread   = g_scheduler->Find(tid);
    if (thread == NULL) return -1;

    Process* process = thread->tinfo->process;

    g_scheduler->Kill(thread);

//    sendKilledMessage();

    //ProcessOperation::freeKernelStack(process->getStackBottom(thread));
    ProcessOperation::freeKernelStack(thread->kernelStackBottom);

    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    delete thread;

    g_scheduler->SwitchToNext();

    /* not reached */

    return NORMAL;
}

void ThreadOperation::sendKilledMessage()
{
    dword threadNum;
    dword* list;
    MessageInfo msg;

    list = g_scheduler->GetAllThreadID(&threadNum);

    /* set message */
    msg.header = MSG_THREAD_KILLED;
    msg.arg1   = g_currentThread->thread->id;

    if (list == NULL) return;

    for (dword i = 0; i < threadNum; i++)
    {

        dword id = list[i];
        if (id == msg.arg1) continue;

        g_messenger->send(id, &msg);

    }

    delete[] list;
}

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
Thread::Thread() : waitEvent(MEvent::NONE), lastCpuUsedTick(0), age(0)
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

dword Process::getStackBottom(Thread* thread)
{
    for (int i = 0; i < threadList_->size(); i++)
    {
        if (threadList_->get(i) != thread) continue;

        return STACK_START - (STACK_SIZE + STACK_SIZE) * i - STACK_SIZE;
    }
    return 0;
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

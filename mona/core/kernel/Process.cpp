/*!
    \filen  Process.cpp
    \brief class Process

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/

#include "global.h"
#include <sys/List.h>
#include <sys/HList.h>
#include <servers/process.h>
#include "Process.h"
#include "PageManager.h"
#include "string.h"
#include <sys/Bitmap.h>
#include "KObjectService.h"
#include "RTC.h"

#define PTR_THREAD(queue) (((Thread*)(queue))->tinfo)

/*----------------------------------------------------------------------
    ProcessOperation
----------------------------------------------------------------------*/
PageManager* ProcessOperation::pageManager;
const int ProcessOperation::USER_PROCESS;
const int ProcessOperation::KERNEL_PROCESS;

static Bitmap* kernelStackMap;

void ProcessOperation::initialize(PageManager* manager)
{
    ProcessOperation::pageManager = manager;

    kernelStackMap = new Bitmap(KERNEL_STACK_SIZE / KERNEL_STACK_UNIT_SIZE, true);
}

LinearAddress ProcessOperation::allocateKernelStack()
{
    int stackIndex = kernelStackMap->find();

    if (stackIndex == -1)
    {
        panic("sorry no kernel stack\n");
        for (;;);
    }

    return KERNEL_STACK_START + stackIndex * KERNEL_STACK_UNIT_SIZE;
}

void ProcessOperation::freeKernelStack(LinearAddress address)
{
    if (address < KERNEL_STACK_START || address > KERNEL_STACK_START + KERNEL_STACK_SIZE)
    {
        ASSERT(false);
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
          result = new UserProcess(name, ProcessOperation::pageManager->createPageDirectory());
          break;
      case KERNEL_PROCESS:
          result = new KernelProcess(name, ProcessOperation::pageManager->createPageDirectory());
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
uint32_t ThreadOperation::id = FIRST_THREAD_ID;
Thread* ThreadOperation::create(Process* process, uint32_t programCounter, uint32_t observer)
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

    process->addThread(thread);
    thread->observers.add(observer);
    return thread;
};

void ThreadOperation::archCreateUserThread(Thread* thread, uint32_t programCounter
                                           , PageEntry* pageDirectory, LinearAddress stack)
{
    /* stack size from 4KB to 4MB */
    PhysicalAddress mappedAddres;
    intptr_t ret = ProcessOperation::pageManager->mapOnePage(pageDirectory,
                                                             mappedAddres,
                                                             stack - 4096,
                                                             PageManager::PAGE_WRITABLE,
                                                             PageManager::PAGE_USER);
    ASSERT(ret == M_OK);

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
    ainfo->esp0    = ProcessOperation::allocateKernelStack() + ProcessOperation::KERNEL_STACK_UNIT_SIZE;  //added by TAKA
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

    thread->stackSegment = new Segment(stack - 4 * 1024 * 1024, 4 * 1024 * 1024);
}

void ThreadOperation::archCreateThread(Thread* thread, uint32_t programCounter
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

intptr_t ThreadOperation::kill(Process* process, Thread* thread, int status)
{
    g_scheduler->Kill(thread);

    sendKilledMessage(status);

    (process->threadNum)--;

    //modified by TAKA
    //ProcessOperation::freeKernelStack(process->getStackBottom(thread));
    ProcessOperation::freeKernelStack(thread->kernelStackBottom - ProcessOperation::KERNEL_STACK_UNIT_SIZE);

    if (process->threadNum < 1)
    {
        KObjectService::cleanupKObjects(process);
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    delete thread;
    return M_OK;
}

intptr_t ThreadOperation::kill(uint32_t tid)
{
    Thread* thread   = g_scheduler->Find(tid);
    if (thread == NULL) {
        return M_BAD_THREAD_ID;
    }
    if (thread->id == FIRST_THREAD_ID) {
        return M_BAD_THREAD_ID;
    }

    Process* process = thread->tinfo->process;

    g_scheduler->Kill(thread);

    ProcessOperation::freeKernelStack(thread->kernelStackBottom);

    (process->threadNum)--;

    if (process->threadNum < 1)
    {
        PageEntry* directory = process->getPageDirectory();
        delete process;
        g_page_manager->returnPhysicalPages(directory);
    }

    delete thread;

    return Scheduler::YIELD;
}

// We don't broadcast MSG_PROCESS_TERMINATED.
// Since it will cause message box overflow easily.
void ThreadOperation::sendKilledMessage(int status)
{
    for (int i = 0; i < g_currentThread->thread->observers.size(); i++) {
        uint32_t dest = g_currentThread->thread->observers[i];

        // for INIT thread.
        if (dest == THREAD_UNKNOWN) {
            continue;
        }
        Thread* thread =  g_scheduler->Find(dest);

        // observer may be dead already.
        if (thread == NULL) {
            continue;
        }
        MessageInfo msg;
        msg.header = MSG_PROCESS_TERMINATED;
        msg.arg1   = g_currentThread->thread->id;
        msg.arg2   = status;
        if (g_messenger->send(thread, &msg) != M_OK) {
            logprintf("Warn %s %s:%d: send failure MSG_PROCESS_TERMINATED\n", __func__, __FILE__, __LINE__);
        }
    }
}

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
uint32_t Process::pid = 0;
Process::Process(const char* name, PageEntry* directory) :
    threadNum(0),
    heap_(Segment(PROCESS_HEAP_START, PROCESS_HEAP_SIZE)),
    kobjects_(HList< Pair<intptr_t, KObject*> >()),
    isUserMode_(false),
    pageDirectory_(directory),
    pid_(++pid_),
    heapStats_(0)
{
    strncpy(name_, name, sizeof(name_));
}

Process::~Process()
{
    /* shared MemorySegment */
    for (int i = 0; i < shared_.size(); i++) {
        SharedMemoryObject* shm = shared_[i]->getSharedMemoryObject();
        shm->detach(g_page_manager, this);
        SharedMemoryObject::destroy(shm);
    }
    /* arguments */
    for (int i = 0; i < arguments_.size(); i++) {
        delete[](arguments_[i]);
    }

    // debug
    if (kobjects_.size() != 0) {
        logprintf("something bad happened %s %s:%d\n", __func__, __FILE__, __LINE__);
        for (int i = 0; i < kobjects_.size(); i++) {
            logprintf("kobject %d\n", kobjects_[i].cdr->getType());
        }
    }

    ASSERT(kobjects_.size() == 0);
}

uint32_t Process::findMainThreadId() const
{
    uint32_t found = THREAD_UNKNOWN;
    for (int i = 0; i < threadList_.size(); i++) {
        uint32_t id = threadList_[i]->id;
        if (id < found) {
            found = id;
        }
    }
    return found;
}

SharedMemorySegment* Process::findSharedSegment(uint32_t id) const
{
    for (int i = 0; i < shared_.size(); i++) {
        SharedMemorySegment* segment = shared_[i];
        if (id == segment->getId()) {
            return segment;
        }
    }
    return NULL;
}


uint32_t Process::getStackBottom(Thread* thread)
{
    for (int i = 0; i < threadList_.size(); i++)
    {
        if (threadList_[i] != thread) continue;

        return STACK_START - (STACK_SIZE + STACK_SIZE) * i - STACK_SIZE;
    }
    return 0;
}

bool Process::hasSharedOverlap(uintptr_t start, uintptr_t end)
{
    for (int i = 0; i < shared_.size(); i++) {
        SharedMemorySegment* sh = shared_[i];
        uintptr_t shStart = sh->getStart();
        uintptr_t shEnd = shStart + sh->getSize();
        if (start <= shStart && shStart <= end) {
            return true;
        } else if (shStart <= start && start <= shEnd) {
            return true;
        }
    }
    return false;
}

void Process::incHeapStats()
{
    heapStats_++;
    if ((heapStats_ * PageManager::ARCH_PAGE_SIZE >= 50 * 1024 * 1024) && (heapStats_ * PageManager::ARCH_PAGE_SIZE % (10 * 1024 * 1024)) == 0) {
        mona_warn("%s heap size is huge = %d MB\n", name_, heapStats_ * PageManager::ARCH_PAGE_SIZE / 1024 / 1024);
    }
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
    Idle function
----------------------------------------------------------------------*/
void monaIdle()
{
    for (;;)
    {
#if 0
        static uint32_t count = 0;
        if (count % 0xFFFFFFF)  g_console->printf(".");
        if (count % 20000000) g_scheduler->dump();
        count++;
#endif
        asm volatile("hlt");
//        arch_idle();
    }
}

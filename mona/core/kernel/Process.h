/*!
    \file  Process.h
    \brief class Process

    class Process

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/
#ifndef _MONA_PROCESS_
#define _MONA_PROCESS_

#include <sys/types.h>
#include <sys/List.h>
#include <sys/BinaryTree.h>
#include "PageManager.h"
#include "Segments.h"
#include "kernel.h"
#include "Mutex.h"
#include "UserSemaphore.h"
#include "KObject.h"
#include "Thread.h"
#include "MemoryAllocator.h"

#define DPL_KERNEL  0
#define DPL_USER    3

class Thread;
class Process;
class MemoryAllocator;
extern "C" VirtualConsole* g_console;

/*----------------------------------------------------------------------
    Idle function
----------------------------------------------------------------------*/
void monaIdle();

/*----------------------------------------------------------------------
    Arch dependent functions
----------------------------------------------------------------------*/
extern "C" void arch_switch_thread_to_user1();
extern "C" void arch_switch_thread_to_user2();
extern "C" void arch_switch_thread_to_v861();
extern "C" void arch_switch_thread_to_v862();
extern "C" void arch_switch_thread1();
extern "C" void arch_switch_thread2();
extern "C" void arch_idle();

/*----------------------------------------------------------------------
    ArchThreadInfo
----------------------------------------------------------------------*/
typedef struct ArchThreadInfo
{
    uint32_t  eip;       // 0
    uint32_t  cs;        // 4
    uint32_t  eflags;    // 8
    uint32_t  eax;       // 12
    uint32_t  ecx;       // 16
    uint32_t  edx;       // 20
    uint32_t  ebx;       // 24
    uint32_t  esp;       // 28
    uint32_t  ebp;       // 32
    uint32_t  esi;       // 36
    uint32_t  edi;       // 40
    uint32_t  ds;        // 44
    uint32_t  es;        // 48
    uint32_t  fs;        // 52
    uint32_t  gs;        // 56
    uint32_t  ss;        // 60
    uint32_t  dpl;       // 64
    uint32_t  esp0;      // 68
    uint32_t  ss0;       // 72
    uint32_t  cr3;       // 76
    uint32_t  fpu[27];   // 80
};

/*----------------------------------------------------------------------
    ThreadInfo
----------------------------------------------------------------------*/
typedef struct ThreadInfo
{
    ArchThreadInfo* archinfo;
    Thread* thread;
    Process* process;
};

/*----------------------------------------------------------------------
    ProcessOperation
----------------------------------------------------------------------*/
class ProcessOperation
{
  public:
    static void initialize(PageManager* manager);
    static Process* create(int type, const char* name);
    static LinearAddress allocateKernelStack();
    static void freeKernelStack(LinearAddress address);

  private:
    enum
    {
        KERNEL_STACK_START     = 0x100000,
        KERNEL_STACK_END       = 0x200000,
        KERNEL_STACK_SIZE      = KERNEL_STACK_END - KERNEL_STACK_START,
        KERNEL_STACK_UNIT_SIZE = 0x1000,
    };

  public:
    static PageManager* pageManager;
    static const int USER_PROCESS   = 1;
    static const int KERNEL_PROCESS = 2;
};

/*----------------------------------------------------------------------
    ThreadOperation
----------------------------------------------------------------------*/
class ThreadOperation
{
  public:
    static Thread* create(Process* process, uint32_t programCounter);
    static int switchThread(bool isProcessChanged, int i);
    static int kill();
    static int kill(uint32_t tid);

  private:
    static void sendKilledMessage();
    static void archCreateUserThread(Thread* thread, uint32_t programCounter, PageEntry* directory, LinearAddress stack);
    static void archCreateThread(Thread* thread, uint32_t programCounter, PageEntry* directory, LinearAddress stack);
    static uint32_t id;
};


/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
class Process
{
  public:
    Process() {}
    Process(const char* name, PageEntry* directory);
    virtual ~Process();

  public:

    uint32_t getStackBottom(Thread* thread);

    inline virtual const char* getName() const
    {
        return name_;
    }

    inline virtual uint32_t getPid()
    {
        return pid_;
    }

    inline virtual bool isUserMode() const
    {
        return isUserMode_;
    }

    inline virtual PageEntry* getPageDirectory() const
    {
        return pageDirectory_;
    }

    inline virtual List<class SharedMemorySegment*>* getSharedList() const
    {
        return shared_;
    }

    inline virtual class HeapSegment* getHeapSegment() const
    {
        return heap_;
    }

    inline virtual List<MessageInfo*>* getMessageList() const
    {
        return messageList_;
    }

    inline virtual List<char*>* getArguments()
    {
        return arguments_;
    }

    inline uint32_t allocateStack() const
    {
        return STACK_START - (STACK_SIZE + STACK_SIZE) * (threadNum - 1);
    }

    inline List<Thread*>* getThreadList() const
    {
        return threadList_;
    }

    inline SharedMemorySegment* getDllSegment() const {
        return dllsegment_;
    }

    inline void* AllocateLinearAddress(uint32_t size) {
        if (this->lallocator == NULL)
        {
            this->lallocator = new MemoryAllocator(0xd0000000, 256 * 1024 * 1024);
        }
        return this->lallocator->Allocate(size);
    }

    static const LinearAddress STACK_START = 0xF0000000;
    static const uint32_t STACK_SIZE          = 0x400000;

  public:
    int threadNum;

  protected:
    static uint32_t pid;
    MemoryAllocator* lallocator;
    List<Thread*>* threadList_;
    List<char*>* arguments_;
    class HeapSegment* heap_;
    class SharedMemorySegment* dllsegment_;
    List<SharedMemorySegment*>* shared_;
    List<MessageInfo*>* messageList_;
    bool isUserMode_;
    PageEntry* pageDirectory_;
    char name_[16];
    uint32_t pid_;
};

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
class UserProcess : public Process
{
  public:
    UserProcess();
    UserProcess(const char* name, PageEntry* directory);
    virtual ~UserProcess();
};

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
class KernelProcess : public Process
{
  public:
    KernelProcess();
    KernelProcess(const char* name, PageEntry* directory);
    virtual ~KernelProcess();
};

/*----------------------------------------------------------------------
    V86Process
----------------------------------------------------------------------*/
class V86Process : public Process
{
  public:
    V86Process();
    V86Process(const char* name, PageEntry* directory);
    virtual ~V86Process();
};

#endif

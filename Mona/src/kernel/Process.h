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
#include "KObject.h"
#include "Thread.h"
//#include "MemoryAllocator.h"

#define DPL_KERNEL  0
#define DPL_USER    3

class Thread;
class Process;
class MemoryAllocator;
extern VirtualConsole* g_console;

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
    dword  eip;       // 0
    dword  cs;        // 4
    dword  eflags;    // 8
    dword  eax;       // 12
    dword  ecx;       // 16
    dword  edx;       // 20
    dword  ebx;       // 24
    dword  esp;       // 28
    dword  ebp;       // 32
    dword  esi;       // 36
    dword  edi;       // 40
    dword  ds;        // 44
    dword  es;        // 48
    dword  fs;        // 52
    dword  gs;        // 56
    dword  ss;        // 60
    dword  dpl;       // 64
    dword  esp0;      // 68
    dword  ss0;       // 72
    dword  cr3;       // 76
    dword  fpu[27];   // 80
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
    static Thread* create(Process* process, dword programCounter);
    static int switchThread(bool isProcessChanged, int i);
    static int kill();
    static int kill(dword tid);

  private:
    static void sendKilledMessage();
    static void archCreateUserThread(Thread* thread, dword programCounter, PageEntry* directory, LinearAddress stack);
    static void archCreateThread(Thread* thread, dword programCounter, PageEntry* directory, LinearAddress stack);
    static dword id;
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

    dword getStackBottom(Thread* thread);

    inline virtual const char* getName() const
    {
        return name_;
    }

    inline virtual dword getPid()
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

    inline dword allocateStack() const
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

/*
    inline void* AllocateLinearAddress(dword size) {
        if (this->lallocator == NULL)
        {
            this->lallocator = new MemoryAllocator(0xd0000000, 256 * 1024 * 1024);
        }
        return this->lallocator->Allocate(size);
    }
*/

    static const LinearAddress STACK_START = 0xF0000000;
    static const dword STACK_SIZE          = 0x400000;

  public:
    int threadNum;

  protected:
    static dword pid;
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
    dword pid_;
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

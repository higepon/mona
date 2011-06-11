/*!
    \file  Process.h
    \brief class Process

    class Process

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
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
#include "Pair.h"
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

    enum
    {
        KERNEL_STACK_START     = 0x100000,
        KERNEL_STACK_END       = 0x200000,
        KERNEL_STACK_SIZE      = KERNEL_STACK_END - KERNEL_STACK_START,
        KERNEL_STACK_UNIT_SIZE = 0x1000,
    };

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
    static Thread* create(Process* process, uint32_t programCounter, uint32_t observer);
    static int switchThread(bool isProcessChanged, int i);
    static intptr_t kill(Process* process, Thread* thread, int status);
    static intptr_t kill(uint32_t tid);

  private:
    static void sendKilledMessage(int status);
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
    Process(const char* name, PageEntry* directory);
    virtual ~Process();

public:
    void incHeapStats();
    bool hasSharedOverlap(uintptr_t start, uintptr_t end);

    uint32_t getStackBottom(Thread* thread);

    int getThreadIndex(Thread* thread)
    {
        for (int i = 0; i < threadList_.size(); i++) {
            if (threadList_[i] == thread) {
                return i;
            }
        }
        return -1;
    }

    virtual const char* getName() const
    {
        return name_;
    }

    virtual uint32_t getPid()
    {
        return pid_;
    }

    virtual bool isUserMode() const
    {
        return isUserMode_;
    }

    virtual PageEntry* getPageDirectory() const
    {
        return pageDirectory_;
    }

    void addSharedMemorySegment(SharedMemorySegment* segment)
    {
        shared_.add(segment);
    }

    void removeSharedMemorySegment(SharedMemorySegment* segment)
    {
        shared_.remove(segment);
    }

    SharedMemorySegment* getSharedMemorySegmentInRange(LinearAddress address)
    {
        for (int i = 0; i < shared_.size(); i++) {
            if (shared_[i]->inRange(address)) {
                return shared_[i];
            }
        }
        return NULL;
    }

    virtual class Segment* getHeapSegment()
    {
        return &heap_;
    }

    void addStartupArgument(char* argument)
    {
        arguments_.add(argument);
    }

    int getNumStartupArguments() const
    {
        return arguments_.size();
    }

    char* getNthStartupArgument(int nth)
    {
        if (nth < arguments_.size()) {
            return arguments_[nth];
        } else {
            return NULL;
        }
    }

    void addThread(Thread* thread)
    {
        threadList_.add(thread);
    }

    void removeThread(Thread* thread)
    {
        threadList_.remove(thread);
    }

    uint32_t findMainThreadId() const;

    uint32_t allocateStack() const
    {
        return STACK_START - (STACK_SIZE + STACK_SIZE) * (threadNum - 1);
    }

    void addKObject(intptr_t id, KObject* obj)
    {
        kobjects_.add(Pair<intptr_t, KObject*>(id, obj));
    }

    bool removeKObject(intptr_t id, KObject* obj)
    {
        return kobjects_.remove(Pair<intptr_t, KObject*>(id, obj));
    }

    SharedMemorySegment* findSharedSegment(uint32_t id) const;

    HList< Pair<intptr_t, KObject*> >* getKObjects()
    {
        return &kobjects_;
    }

    static const LinearAddress STACK_START = 0xF0000000;
    static const uint32_t STACK_SIZE          = 0x400000;

  public:
    int threadNum;

  protected:
    static uint32_t pid;
    HList<Thread*> threadList_;
    HList<char*> arguments_;
    class Segment heap_;
    HList<SharedMemorySegment*> shared_;
    HList< Pair<intptr_t, KObject*> > kobjects_;
    bool isUserMode_;
    PageEntry* pageDirectory_;
    char name_[16];
    uint32_t pid_;
    uint32_t heapStats_;
};

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
class UserProcess : public Process
{
private:
    UserProcess();
public:
    UserProcess(const char* name, PageEntry* directory);
    virtual ~UserProcess();
};

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
class KernelProcess : public Process
{
private:
    KernelProcess();
public:
    KernelProcess(const char* name, PageEntry* directory);
    virtual ~KernelProcess();
};

#endif

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

#include <types.h>
#include <PageManager.h>
#include <Segments.h>
#include <collection.h>
#include <kernel.h>
#include <Mutex.h>

#define DPL_KERNEL  0
#define DPL_USER    3

class Thread;
class Process;
extern VirtualConsole*g_console;


/*----------------------------------------------------------------------
    schedule
----------------------------------------------------------------------*/
void schedule(bool tick);

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
typedef struct ArchThreadInfo {
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
};

/*----------------------------------------------------------------------
    ThreadInfo
----------------------------------------------------------------------*/
typedef struct ThreadInfo {
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

  private:
    static const LinearAddress KERNEL_STACK_START     = 0x100000;
    static const LinearAddress KERNEL_STACK_UNIT_SIZE = 0x1000;

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
    static int switchThread(bool isProcessChanged);

  private:
    static void archCreateUserThread(Thread* thread, dword programCounter, PageEntry* directory, LinearAddress stack);
    static void archCreateThread(Thread* thread, dword programCounter, PageEntry* directory, LinearAddress stack);
};

class Scheduler
{
public:
    Scheduler();
    virtual ~Scheduler();

public:
    bool schedule();
    void join(Thread* thread, int priority = 30);
    int kill(Thread* thread);
    int wait(Thread* thread, int waitReason);
    int wakeup(Thread* thread, int waitReason);

private:
    int calcPriority(Thread* thread);

protected:
    Array<Thread> runq;
    Array<Thread> waitq;
    dword tickTotal;
    int monaMin;
};


/*----------------------------------------------------------------------
    Queue
----------------------------------------------------------------------*/
class Queue
{
public:
    static void initialize(Queue* queue);
    static void addToNext(Queue* p, Queue* q);
    static void addToPrev(Queue* p, Queue* q);
    static void remove(Queue* p);
    static bool isEmpty(Queue* p);
    static Queue* removeNext(Queue* p);
    static Queue* top(Queue* root);
public:
    Queue* next;
    Queue* prev;
};

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
class Thread : public Queue {

  public:
    Thread();
    virtual ~Thread();

  public:
    inline void tick() {
        tick_++;
        timeLeft_--;
    }

    inline void tick(dword tick) {
        tick_     += tick;
        timeLeft_ -= tick;
    }

    inline dword getTick() const {
        return tick_;
    }

    inline bool hasTimeLeft() const {
        return (timeLeft_ > 0);
    }

    inline void setTimeLeft(long timeLeft) {
        timeLeft_ = timeLeft;
    }

    inline ThreadInfo* getThreadInfo() const {
        return threadInfo_;
    }

    inline void setWaitReason(int reason) {
        waitReason_ = reason;
    }

    inline int getWaitReason() const {
        return waitReason_;
    }

  private:

    /* this is public , because of getXXX */
    ThreadInfo* threadInfo_;

  public:
    int currPriority;
    int basePriotity;
    int totalTick;
    int partTick;
    dword scheduled;

  protected:
    dword tick_;
    dword timeLeft_;
    int waitReason_;
};


/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
class Process {

  public:
    Process() {}
    Process(const char* name, PageEntry* directory);
    virtual ~Process();

  public:
    inline virtual const char* getName() const {
        return name_;
    }

    inline virtual void setTimeLeft(long timeLeft) {
        timeLeft_ = timeLeft;
    }

    inline virtual void tick() {
        tick_++;
        timeLeft_ --;
    }

    inline virtual void tick(dword tick) {
        tick_     += tick;
        timeLeft_ -= tick;
    }

    inline virtual dword getTick() {
        return tick_;
    }

    inline virtual dword getPid() {
        return pid_;
    }

    inline virtual bool hasTimeLeft() const {
        return timeLeft_ > 0;
    }

    inline virtual bool isUserMode() const {
        return isUserMode_;
    }

    inline virtual PageEntry* getPageDirectory() const {
        return pageDirectory_;
    }

    inline virtual List<class SharedMemorySegment*>* getSharedList() const {
        return shared_;
    }

    inline virtual class HeapSegment* getHeapSegment() const {
        return heap_;
    }

    inline virtual List<MessageInfo*>* getMessageList() const {
        return messageList_;
    }

    inline virtual BinaryTree<class KMutex*>* getKMutexTree() const {
        return kmutexTree_;
    }

    inline virtual void setWakeupTimer(dword timer) {
        wakeupTimer_ = timer;
    }

    inline virtual dword getWakeupTimer() const {
        return wakeupTimer_;
    }

    inline virtual KMutex* getKMutex(int id) {
        return kmutexTree_->get(id);
    }

    inline virtual List<char*>* getArguments() {
        return arguments_;
    }

    inline dword allocateStack() const {
        return STACK_START - STACK_SIZE * (threadNum - 1);
    }


    static const LinearAddress STACK_START = 0xEFFFFFFF;
    static const dword STACK_SIZE          = 0x1000;

  public:
    int threadNum;

  protected:
    static dword pid;
    List<char*>* arguments_;
    class HeapSegment* heap_;
    List<SharedMemorySegment*>* shared_;
    List<MessageInfo*>* messageList_;
    BinaryTree<KMutex*>* kmutexTree_;
    bool isUserMode_;
    PageEntry* pageDirectory_;
    char name_[16];
    //    OutputStream* stdout;
    //    OutputStream* stderr;
    //    InputStream*  stdin;
    dword tick_;
    dword wakeupTimer_;
    long  timeLeft_;
    dword pid_;
    byte priority_;
    byte state_;
};

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
class UserProcess : public Process {

  public:
    UserProcess();
    UserProcess(const char* name, PageEntry* directory);
    virtual ~UserProcess();
};

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
class KernelProcess : public Process {

  public:
    KernelProcess();
    KernelProcess(const char* name, PageEntry* directory);
    virtual ~KernelProcess();
};

/*----------------------------------------------------------------------
    V86Process
----------------------------------------------------------------------*/
class V86Process : public Process {

  public:
    V86Process();
    V86Process(const char* name, PageEntry* directory);
    virtual ~V86Process();
};

#endif

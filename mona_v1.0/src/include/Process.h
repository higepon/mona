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
};

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
class Thread {

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

  protected:
    dword tick_;
    dword timeLeft_;
    int waitReason_;
};

/*----------------------------------------------------------------------
    ThreadManager
----------------------------------------------------------------------*/
class ThreadManager {

  public:
    ThreadManager(bool isUser);
    ThreadManager(bool isUser, bool isV86);
    virtual ~ThreadManager();

  public:
    Thread* create(dword programCounter, PageEntry* pageDirectory);
    int join(Thread* thread);
    int killAllThread();
    int kill(Thread* thread);
    int wait(Thread* thread, int waitReason);
    int wakeup(int waitReason);
    Thread* schedule(bool tick);
    void printAllThread();

    inline Thread* getCurrentThread() const {
        return current_;
    }

    inline bool hasActiveThread() const {
        return dispatchList_->size();
    }

    inline int switchThread(bool isProcessChanged, bool isUser) const {

        isUser = isUser && (current_->getThreadInfo()->archinfo->cs & 0x03);

        if (isProcessChanged && isV86_) {

            arch_switch_thread_to_v862();
        } else if (!isProcessChanged && isV86_) {

            arch_switch_thread_to_v861();
        } else if (isProcessChanged && isUser) {

            /* address space & therad switch */
            arch_switch_thread_to_user2();
        } else if (!isProcessChanged && isUser) {

            /* only thread switch */
            arch_switch_thread_to_user1();
        } else if (isProcessChanged && !isUser) {

            /* address space & therad switch */
            arch_switch_thread2();
        } else {
            arch_switch_thread1();
        }

        /* does not come here */
        return NORMAL;
    }

    inline void waitMutex(Thread* thread) {
        dispatchList_->remove(thread);
    }

    inline void activateMutex(Thread* thread) {
        dispatchList_->add(thread);
    }

  public:
    static void setup();

  private:
    inline dword allocateStack() const {
        return STACK_START - STACK_SIZE * (threadCount - 1);
    }
    void archCreateUserThread(Thread* thread, dword programCounter, PageEntry* directory) const;
    void archCreateThread(Thread* thread, dword programCounter, PageEntry* directory) const;
    void archCreateV86Thread(Thread* thread, dword programCounter, PageEntry* pageDirectory) const;
    int kill(List<Thread*>* list);

  private:
    Thread* current_;
    List<Thread*>* dispatchList_;
    List<Thread*>* waitList_;
    bool isUser_;
    bool isV86_;

  private:
    static const LinearAddress STACK_START = 0xEFFFFFFF;
        //    static const LinearAddress STACK_START = 0xFFFFFFFF; vmware hate this.
    static const dword STACK_SIZE          = 4 * 1024;
    int threadCount;

  public:
    static const int WAIT_MUTEX = 0;
    static const int WAIT_SLEEP = 1;
    static const int WAIT_NONE  = 2;
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

    inline virtual bool hasActiveThread() const {
        return threadManager_->hasActiveThread();
    }

    inline virtual bool isUserMode() const {
        return isUserMode_;
    }

    inline virtual int switchThread(bool isProcessChanged, bool isUser) const {
        threadManager_->switchThread(isProcessChanged, isUser);
        return NORMAL;
    }

    inline virtual void printAllThread() {
        threadManager_->printAllThread();
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

    inline virtual int killSelf() {
        return threadManager_->killAllThread();
    }

    inline virtual void setWakeupTimer(dword timer) {
        wakeupTimer_ = timer;
    }

    inline virtual dword getWakeupTimer() const {
        return wakeupTimer_;
    }

    inline virtual void waitMutex(Thread* thread) {
        threadManager_->waitMutex(thread);
    }

    inline virtual void activateMutex(Thread* thread) {
        threadManager_->activateMutex(thread);
    }

    inline virtual KMutex* getKMutex(int id) {
        return kmutexTree_->get(id);
    }

    inline virtual List<char*>* getArguments() {
        return arguments_;
    }

    inline virtual int wait(Thread* thread, int waitReason) {
        return threadManager_->wait(thread, waitReason);
    }

    inline virtual int wakeup(int waitReason) {
        return threadManager_->wakeup(waitReason);
    }

    virtual int join(Thread* thread);
    virtual Thread* schedule(bool tick);
    virtual Thread* createThread(dword programCounter);

  protected:
    static dword pid;
    List<char*>* arguments_;
    class HeapSegment* heap_;
    List<SharedMemorySegment*>* shared_;
    List<MessageInfo*>* messageList_;
    BinaryTree<KMutex*>* kmutexTree_;
    bool isUserMode_;
    ThreadManager* threadManager_;
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

/*----------------------------------------------------------------------
    ProcessManager
----------------------------------------------------------------------*/
class ProcessManager {

  public:
    ProcessManager(PageManager* pageManager);
    virtual ~ProcessManager();

  public:
    Process* create(int type, const char* name);
    Thread* createThread(Process* process, dword programCounter);
    int join(Process* process, Thread* thread);
    int add(Process* process);
    int kill(Process* process);
    void killSelf();
    int sleep(Process* process, dword tick);
    int switchProcess();
    int wait(Process* process, Thread* thread, int waitReason);
    bool schedule(bool tick);
    inline Process* getCurrentProcess() const {
        return current_;
    }

    inline dword getTick() const {
        return tick_;
    }

    inline void tick() {
        tick_++;
    }

    LinearAddress allocateKernelStack() const;
    void printProcess();
    Process* find(const char* name);
    Process* find(dword pid);
    dword lookup(const char* name);
    void wakeup();
    int wakeup(Process* process, int waitReason);

  public:
    static const int USER_PROCESS   = 0;
    static const int KERNEL_PROCESS = 1;
    static const int V86_PROCESS    = 2;

  private:
    dword tick_;
    List<Process*>* dispatchList_;
    List<Process*>* waitList_;
    PageManager* pageManager_;
    Process* current_;
    Process* idle_;
    bool isProcessChanged_;
};

#endif

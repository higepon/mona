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

#define MAX_PROCESS 512
#define DPL_KERNEL  0
#define DPL_USER    3

typedef struct ProcessInfo {
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
    dword  tick;
    dword  pid;
    dword  wakeupTimer;
    int    state;
    class  Process* process;
    ProcessInfo* prev;
    ProcessInfo* next;
    class StackSegment* stack;
    class SharedMemorySegment* shared;
    class HeapSegment* heap;
    char  name[16];
    Message* message;
    List<Message*>* messageList;
};

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
        timeLeft_ += tick;
    }

    inline dword getTick() const {
        return tick_;
    }

    inline bool hasTimeLeft() const {
        return (timeLeft_ > 0);
    }

  protected:
    dword tick_;
    dword timeLeft_;
};

/*----------------------------------------------------------------------
    ThreadScheduler
----------------------------------------------------------------------*/
class ThreadScheduler {

  public:
    ThreadScheduler();
    virtual ~ThreadScheduler();

  public:
    Thread* schedule(Thread* current);
    int join(Thread* thread);
    int kill(Thread* thread);

  private:
    List<Thread*>* list_;
};

/*----------------------------------------------------------------------
    ThreadManager
----------------------------------------------------------------------*/
class ThreadManager {

  public:
    ThreadManager(PageManager* pageManager);
    virtual ~ThreadManager();

  public:
    Thread* create(dword programCounter);
    int join(Thread* thread);
    int kill(Thread* thread);
    int switchThread();
    Thread* schedule();
    Thread* getCurrentThread() const {
        return current_;
    }

  private:
    ThreadScheduler* scheduler_;
    PageManager* pageManager_;
    Thread* current_;
    Thread* idle_;
};

/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
class Process_ {

  public:
    Process_() {}
    Process_(const char* name);
    virtual ~Process_();

  public:
    inline const char* getName() const {return name_;}
    inline void tick() {
        tick_++;
        timeLeft_ --;
    }
    inline void tick(dword tick) {
        tick_     += tick;
        timeLeft_ -= tick;
    }

    inline dword getTick() {return tick_;}
    inline bool hasTimeLeft() const {return timeLeft_ > 0;}
    int join(Thread* thread);
    Thread* createThread(dword programCounter);

  protected:
    bool isKernelMode_;
    ThreadManager* threadManager_;
    char name_[16];
    //    OutputStream* stdout;
    //    OutputStream* stderr;
    //    InputStream*  stdin;
    dword tick_;
    long  timeLeft_;
    dword pid_;
    byte priority_;
    byte state_;
};

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
class UserProcess_ : public Process_ {

  public:
    UserProcess_();
    UserProcess_(const char* name);
    virtual ~UserProcess_();
};

/*----------------------------------------------------------------------
    KernelInfo
----------------------------------------------------------------------*/
class KernelProcess_ : public Process_ {

  public:
    KernelProcess_();
    KernelProcess_(const char* name);
    virtual ~KernelProcess_();
};

/*----------------------------------------------------------------------
    ProcessScheduler
----------------------------------------------------------------------*/
class ProcessScheduler {

  public:
    ProcessScheduler(Process_* idle);
    virtual ~ProcessScheduler();

  public:
    Process_* schedule(Process_* current);
    int add(Process_* process);
    int kill(Process_* process);
    bool hasProcess(Process_* process) const;

  private:
    List<Process_*>* list_;
    Process_* idle_;

};

/*----------------------------------------------------------------------
    ProcessManager
----------------------------------------------------------------------*/
class ProcessManager_ {

  public:
    ProcessManager_(PageManager* pageManager);
    virtual ~ProcessManager_();

  public:
    Process_* create(int type, const char* name);
    Thread* createThread(Process_* process, dword programCounter);
    int join(Process_* process, Thread* thread);
    int add(Process_* process);
    int kill(Process_* process);
    int switchProcess();
    bool schedule();
    Process_* getCurrentProcess() const;
    bool hasProcess(Process_* process) const;

  public:
    static const int USER_PROCESS   = 0;
    static const int KERNEL_PROCESS = 1;

  private:
    ProcessScheduler* scheduler_;
    PageManager* pageManager_;
    Process_* current_;
    Process_* idle_;

};

/*!
    class Process
*/
class Process {

  public:
    Process(const char*);

    Process() {}
    virtual ~Process();

  protected:

  public:
    ProcessInfo pinfo_;

    virtual void setup(virtual_addr entryPoint, virtual_addr stack, virtual_addr kernel_stack, PageEntry* pagedir, dword pid);

    static void setup();

  public:
    static const int RUNNING;
    static const int READY;
    static const int SLEEPING;
};

/*!
    class UserProcess
*/
class UserProcess : public Process {

  public:
    UserProcess(const char*);

    virtual ~UserProcess() {
    }


    //    void setup(virtual_addr entryPoint, virtual_addr stack, PTE* pagedir, dword pid);

};

/*!
    class V86Process
*/
class V86Process : public Process {

  public:
    V86Process(const char*);

    virtual ~V86Process() {
    }


    //    void setup(virtual_addr entryPoint, virtual_addr stack, PTE* pagedir, dword pid);

};

#endif

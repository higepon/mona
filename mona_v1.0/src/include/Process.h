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

typedef struct ProcessInfo_ {
    //    ThreadInfo_ threadList;
    char name[16];
    //    OutputStream* stdout;
    //    OutputStream* stderr;
    //    InputStream*  stdin;
    dword timeLeft;
    dword tick;
    dword pid;
    byte mode;
    byte priority;
    byte state;
};

class ProcessScheduler {

  public:
    ProcessScheduler();
    virtual ~ProcessScheduler();

  public:
    ProcessInfo_* schedule(ProcessInfo_* current);
    int addProcess(ProcessInfo_* pinfo);
    int kill(ProcessInfo_* pinfo);

  private:
    List<ProcessInfo_*>* list_;

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

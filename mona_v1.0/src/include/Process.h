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

#include<types.h>
#include<PagingUtil.h>

#define MAX_PROCESS 512
#define DPL_KERNEL  0
#define DPL_USER    3

typedef struct ProcessInfo {
    dword  eip;
    dword  cs;
    dword  eflags;
    dword  eax;
    dword  ecx;
    dword  edx;
    dword  ebx;
    dword  esp;
    dword  ebp;
    dword  esi;
    dword  edi;
    dword  ds;
    dword  ss;
    dword  cr3;
    dword  tick;
    dword  pid;
    dword  dpl;
    dword  wakeupTimer;
    int    state;
    class  Process* process;
    ProcessInfo* prev;
    ProcessInfo* next;
    char  name[16];
};

/*!
    class Process
*/
class Process {

  public:
    Process(const char*);

    Process() {}

    virtual ~Process() {
    }

  protected:

  public:
    ProcessInfo pinfo_;

    void setup(virtual_addr entryPoint, virtual_addr stack, PTE* pagedir, dword pid);

    static void setup();

  public:
    static const int RUNNING;
    static const int READY;
    static const int SLEEPING;
};

/*!
    class UserProcess
*/
class UserProcess : Process {

  public:
    UserProcess(const char*);

    virtual ~UserProcess() {
    }


    void setup(virtual_addr entryPoint, virtual_addr stack, PTE* pagedir, dword pid);

};

#endif

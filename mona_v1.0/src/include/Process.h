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

typedef struct {
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
    dword  tick;
    dword  pid;
    class  Process* process;
} ProcessInfo;

/*!
    class Process
*/
class Process {

  public:
    Process() {

        pinfo_.process = this;
    }
    virtual ~Process() {
    }
    virtual int main();
    virtual void init();
    virtual void  destroy();

  public:
    ProcessInfo pinfo_;
};

#endif

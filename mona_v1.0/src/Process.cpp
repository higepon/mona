/*!
    \file  Process.cpp
    \brief class Process

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/

#include<Process.h>
#include<global.h>

/*!
    \brief main

    apply template method pattern

    \author HigePon
    \date   create:2003/06/28 update:
*/
int Process::main() {

    int error;

    init();
    error = execute();
    destroy();

    return error;
}

/*!
    \brief init

    \author HigePon
    \date   create:2003/06/28 update:
*/
void Process::init() {


}

/*!
    \brief destroy

    \author HigePon
    \date   create:2003/06/28 update:
*/
void Process::destroy() {


}

extern "C" Process** g_process;
/*!
    \brief setup

    \author HigePon
    \date   create:2003/06/28 update:
*/
void Process::setup() {

    g_process = new Process*[MAX_PROCESS];
    return;
}


void KernelProcess::init() {

    g_console->printf("KernelProcess:init()\n");
    pinfo_.cs     = 0x08;
    pinfo_.eflags = 0x0200;
    pinfo_.esp    = 0x800000;
    pinfo_.ebp    = 0x800000;
    //    pinfo_.eip    = (dword)(&(KernelProcess::main));

}

void KernelProcess::destroy() {

    g_console->printf("KernelProcess:destroy()\n");
    while (true);

}

int KernelProcess::execute() {

    g_console->printf("KernelProcess:execute()\n");
    return 0;
}


//  void Kernel::Main(ProcessInfo* process) {

//      process = new KernelProcess();
//      process->main();
//  }

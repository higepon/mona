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

void Process::setEntryPoint(virtual_addr point) {

    pinfo_.eip = (dword)point;
    return;
}

Process::Process(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = 0x08;
    pinfo_.eflags  = 0x200;
    pinfo_.eax     = 0;
    pinfo_.ecx     = 0;
    pinfo_.edx     = 0;
    pinfo_.ebx     = 0;
    pinfo_.esi     = 0;
    pinfo_.edi     = 0;
    pinfo_.tick    = 0;
    pinfo_.dpl     = DPL_KERNEL;
}

void Process::setup(dword stack, dword pid) {

    pinfo_.esp = stack;
    pinfo_.ebp = stack;
    pinfo_.pid = pid;
    return;
}

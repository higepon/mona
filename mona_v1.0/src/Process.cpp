/*!
    \file  Process.cpp
    \brief class Process

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/

#include <global.h>
#include <PageManager.h>

extern "C" Process** g_process;

const int Process::RUNNING  = 0;
const int Process::READY    = 1;
const int Process::SLEEPING = 2;

/*!
    \brief setup

    \author HigePon
    \date   create:2003/06/28 update:
*/
void Process::setup() {

    g_process = new Process*[MAX_PROCESS];

    Process* kernel = new Process("Kernel");
    g_current_process = &(kernel->pinfo_);

    return;
}

Process::Process(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = KERNEL_CS;
    pinfo_.ds      = KERNEL_DS;
    pinfo_.ss      = KERNEL_SS;
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

void Process::setup(virtual_addr entryPoint, virtual_addr stack, virtual_addr kernel_stack, PageEntry* pagedir, dword pid) {

    pinfo_.eip  = (dword)entryPoint;
    pinfo_.esp  = (dword)stack;
    pinfo_.ebp  = (dword)stack;
    pinfo_.cr3  = (dword)pagedir;
    pinfo_.pid  = pid;
    pinfo_.ss0  = KERNEL_SS;
    pinfo_.esp0 = (dword)kernel_stack;
    return;
}

UserProcess::UserProcess(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = USER_CS;
    pinfo_.ds      = USER_DS;
    pinfo_.ss      = USER_SS;
    pinfo_.eflags  = 0x200;
    pinfo_.eax     = 0;
    pinfo_.ecx     = 0;
    pinfo_.edx     = 0;
    pinfo_.ebx     = 0;
    pinfo_.esi     = 0;
    pinfo_.edi     = 0;
    pinfo_.tick    = 0;
    pinfo_.dpl     = DPL_USER;
}

V86Process::V86Process(const char* name) {

    pinfo_.process = this;
    strncpy(pinfo_.name, name, sizeof(pinfo_.name));

    pinfo_.cs      = USER_CS;
    pinfo_.ds      = USER_DS;
    pinfo_.ss      = USER_SS;
    pinfo_.eflags  = 0x20200;
    pinfo_.eax     = 0;
    pinfo_.ecx     = 0;
    pinfo_.edx     = 0;
    pinfo_.ebx     = 0;
    pinfo_.esi     = 0;
    pinfo_.edi     = 0;
    pinfo_.tick    = 0;
    pinfo_.dpl     = DPL_USER;
}

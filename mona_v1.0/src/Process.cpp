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

int Process::execute() {

    return 0;
}

void Process::setEntryPoint(virtual_addr point) {

    pinfo_.eip = (dword)point;
    return;
}

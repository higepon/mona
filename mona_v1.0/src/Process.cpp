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

/*!
    \brief main

    \author HigePon
    \date   create:2003/06/28 update:
*/
int Process::main() {


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

extern "C" Process* g_process;
/*!
    \brief setup

    \author HigePon
    \date   create:2003/06/28 update:
*/
void Process::setup() {

    g_process = new Process[MAX_PROCESS];
    return;
}

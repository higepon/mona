/*!
    \file  disp.cpp
    \brief disp function

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include<monaKernel.h>
#include<kthread.h>
#include<disp.h>
#include<io.h>
#include<pic.h>
#include<semaphore.h>
#include<global.h>

static semaphore sem = 1;

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:2003/03/21
*/
void disp_name1() {

    while (true) {
        disableTimer();
        g_console->printf("a");
        enableTimer();
    }
}

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void disp_name2() {

    while (true) {
        disableTimer();
        g_console->printf("b");
        enableTimer();
   }
}

void disp_name3() {


    while (true) {
        disableTimer();
        g_console->printf("c");
        enableTimer();
    }
}
void disp_name4() {


    while (true) {
        disableTimer();
        g_console->printf("d");
        enableTimer();
    }
}

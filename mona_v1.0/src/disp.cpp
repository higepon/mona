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

extern "C" void write_font(int a, char b, char c);
extern "C" char pos_x;
extern "C" char pos_y;

static semaphore sem = 1;

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:2003/03/21
*/
void disp_name1() {

    dword color = 0;

    while (true) {

        //        if (g_kthread_current->tick % 500) continue;
        disableTimer();

        int x = pos_x;
        int y = pos_y;

        pos_x = 42, pos_y = 0;
        write_font('M', color%16, 0);
        color++;
        pos_x = x;
        pos_y = y;
        enableTimer();
    }
}

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void disp_name2() {

    dword color = 0;

    while (true) {

        //        if (g_kthread_current->tick % 50) continue;
        disableTimer();

        int x = pos_x;
        int y = pos_y;

        pos_x = 43, pos_y = 0;
        write_font('o', color%16, 0);
        color++;
        pos_x = x;
        pos_y = y;
        enableTimer();
   }
}

void disp_name3() {

    dword color = 0;

    while (true) {
        //        if (g_kthread_current->tick % 50) continue;
        disableTimer();

        int x = pos_x;
        int y = pos_y;

        pos_x = 44, pos_y = 0;
        write_font('n', color%16, 0);
        color++;
        pos_x = x;
        pos_y = y;
        enableTimer();
    }
}

void disp_name4() {

    dword color = 0;

    while (true) {
        //        if (g_kthread_current->tick % 50) continue;
        disableTimer();

        int x = pos_x;
        int y = pos_y;

        pos_x = 45, pos_y = 0;
        write_font('a', color%16, 0);
        color++;
        pos_x = x;
        pos_y = y;
        enableTimer();
    }
}

void disp_kthread_info() {


    while (true) {

        disableTimer();

        int x = pos_x;
        int y = pos_y;

         pos_x = 0, pos_y = 16;

         g_console->printf("kernel thread Information \n");
         g_console->printf("thread total number %d \n" , g_kthreadInfo.threadNum);
         g_console->printf("kernel total %d[tick] \n", g_kthreadInfo.tick);
         g_console->printf("[idle] %d%% [kernel] %d%%"
                           , (g_kthread_idle->tick) * 100 / (g_kthreadInfo.tick)
                           , 100 - (g_kthread_idle->tick) * 100 / (g_kthreadInfo.tick));

         pos_x = x;
         pos_y = y;
         enableTimer();
    }
}

void kthread_idle() {

    while (true) {

        asm volatile("nop");
    }
}

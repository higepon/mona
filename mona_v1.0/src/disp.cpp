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
        //        if (g_kthread_current->tick % 50) continue;
        disableTimer();

        disp_write_font(76, 0, 'M', color%16);

        enableTimer();
        color++;
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

        disp_write_font(77, 0, 'o', color%16);

        enableTimer();
        color++;
    }
}

void disp_name3() {

    dword color = 0;

    while (true) {
        //        if (g_kthread_current->tick % 50) continue;
        disableTimer();

        disp_write_font(78, 0, 'n', color%16);

        enableTimer();
        color++;
    }
}

void disp_name4() {

    dword color = 0;

    while (true) {
        //        if (g_kthread_current->tick % 50) continue;
        disableTimer();

        disp_write_font(79, 0, 'a', color%16);

        enableTimer();
        color++;
    }
}

void disp_write_font(int x, int y, char ch, byte color) {

    int tempx = pos_x;
    int tempy = pos_y;

    pos_x = x;
    pos_y = y;

    write_font(ch, color, 0);

    pos_x = tempx;
    pos_y = tempy;
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

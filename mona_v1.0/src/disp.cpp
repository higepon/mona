/*!
    \file  disp.cpp
    \brief disp function

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include<kernel.h>
#include<kthread.h>
#include<disp.h>
#include<io.h>
#include<pic.h>
#include<semaphore.h>
#include<global.h>
#include<syscalls.h>
#include<rtc.h>
#include<GraphicalConsole.h>

extern "C" void write_font(int a, char b, char c);
extern "C" void put_pixel(int pixel_x, int pixel_y, char color);
extern "C" char pos_x;
extern "C" char pos_y;

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:2003/03/21
*/
void disp_name1() {

    dword color = 0;

    while (true) {

        while (Semaphore::down(&g_semaphore_console)) syscall_kthread_yield();

        disp_write_font(75, 0, 'M', color%13);

        Semaphore::up(&g_semaphore_console);

        color++;
        syscall_kthread_yield();
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
        //        while (Semaphore::down(&g_semaphore_console)) syscall_kthread_yield();

        disp_write_font(76, 0, 'o', color%14);

        //        Semaphore::up(&g_semaphore_console);
        color++;
        //        syscall_kthread_yield();
    }
}

void disp_name3() {

    dword color = 0;

    while (true) {

        //while (Semaphore::down(&g_semaphore_console)) syscall_kthread_yield();

        disp_write_font(77, 0, 'n', color%15);


        //Semaphore::up(&g_semaphore_console);
        color++;
        //syscall_kthread_yield();
    }
}

void disp_name4() {

    dword color = 0;

    while (true) {

        while (Semaphore::down(&g_semaphore_console)) syscall_kthread_yield();

        disp_write_font(78, 0, 'a', color%16);

        Semaphore::up(&g_semaphore_console);
        color++;

        syscall_kthread_yield();
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

        while (Semaphore::down(&g_semaphore_console)) syscall_kthread_yield();

        int x = pos_x;
        int y = pos_y;

         pos_x = 0, pos_y = 22;

         g_console->printf("   *  __/\x18/\x18____   *    -----kernel thread Information -----\n");
         g_console->printf("*    /_(*ß°ß)_ /\x18    *  thread total number : %d[threads]\n", g_kthreadInfo.threadNum);
         g_console->printf("  * /___U__U_/|\x18/ *     System total tick   : %d[ticks]\n", g_kthreadInfo.tick);
         g_console->printf(" *   |________|/    *   thread yield        : %d[times]\n", g_kthreadInfo.yield);
         g_console->printf("                        [idle] %d%%          : [kernel] %d%% \n"
                           , (g_kthread_idle->tick) * 100 / (g_kthreadInfo.tick)
                           , 100 - (g_kthread_idle->tick) * 100 / (g_kthreadInfo.tick));

//      Date date;
//      rtc_get_date(&date);
//      g_console->printf("%d %d/%d %d:%d %d\n", (dword)(date.year), (dword)(date.month), (dword)(date.day), (dword)(date.hour), (dword)(date.min), (dword)(date.sec));

         pos_x = x;
         pos_y = y;
         Semaphore::up(&g_semaphore_console);

         syscall_kthread_yield();
    }
}

void kthread_idle() {

    while (true) {

        asm volatile("nop");
    }
}

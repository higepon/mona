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
#include<syscalls.h>
#include<rtc.h>

extern "C" void write_font(int a, char b, char c);
extern "C" void put_pixel(int pixel_x, int pixel_y, char color);
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

        while (semaphore_down(&sem)) syscall_kthread_yield();

        disp_write_font(75, 0, 'M', color%13);

        semaphore_up(&sem);

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
        while (semaphore_down(&sem)) syscall_kthread_yield();

        disp_write_font(76, 0, 'o', color%14);

        semaphore_up(&sem);
        color++;
        syscall_kthread_yield();
    }
}

void disp_name3() {

    dword color = 0;

    while (true) {

        while (semaphore_down(&sem)) syscall_kthread_yield();

        disp_write_font(77, 0, 'n', color%15);


        semaphore_up(&sem);
        color++;
        syscall_kthread_yield();
    }
}

void disp_name4() {

    dword color = 0;

    while (true) {

        while (semaphore_down(&sem)) syscall_kthread_yield();

        disp_write_font(78, 0, 'a', color%16);

        semaphore_up(&sem);
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

        while (semaphore_down(&sem)) syscall_kthread_yield();

        int x = pos_x;
        int y = pos_y;

         pos_x = 0, pos_y = 22;

         g_console->printf("  ___/Í/Í____ -----kernel thread Information -----\n");
         g_console->printf(" /__(*ßoß)_ /|thread total number : %d[threads]\n", g_kthreadInfo.threadNum);
         g_console->printf("|    U~~U  | |System total tick   : %d[ticks]\n", g_kthreadInfo.tick);
         g_console->printf("|    Mona  | |thraed yield        : %d[times]\n", g_kthreadInfo.yield);
         g_console->printf("|__________|/ [idle] %d%%         : [kernel] %d%% \n"
                           , (g_kthread_idle->tick) * 100 / (g_kthreadInfo.tick)
                           , 100 - (g_kthread_idle->tick) * 100 / (g_kthreadInfo.tick));

//      Date date;
//      rtc_get_date(&date);
//      g_console->printf("%d %d/%d %d:%d %d\n", (dword)(date.year), (dword)(date.month), (dword)(date.day), (dword)(date.hour), (dword)(date.min), (dword)(date.sec));

         pos_x = x;
         pos_y = y;
         semaphore_up(&sem);

         syscall_kthread_yield();
    }
}

void kthread_idle() {

    while (true) {

        asm volatile("nop");
    }
}


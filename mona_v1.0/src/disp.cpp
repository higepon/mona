/*!
    \file  disp.cpp
    \brief disp function

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include <kernel.h>
#include <disp.h>
#include <io.h>
#include <pic.h>
#include <Semaphore.h>
#include <global.h>
#include <syscalls.h>
#include <userlib.h>
#include <rtc.h>
#include <GraphicalConsole.h>
#include <Message.h>

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

        while (Semaphore::down(&g_semaphore_console)) syscall_sleep(8);

        disp_write_font(75, 0, 'M', color%13);

        Semaphore::up(&g_semaphore_console);
        int result = syscall_sleep(40);
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

        while (Semaphore::down(&g_semaphore_console)) syscall_sleep(2);

        disp_write_font(76, 0, 'o', color%14);

        Semaphore::up(&g_semaphore_console);
        int result = syscall_sleep(50);
        color++;
    }
}

void disp_name3() {

    dword color = 0;

    while (true) {

        while (Semaphore::down(&g_semaphore_console)) syscall_sleep(3);

        disp_write_font(77, 0, 'n', color%13);

        Semaphore::up(&g_semaphore_console);

        Message msg;
        //        g_message_server->send(&msg);
        int result = syscall_sleep(300);

        color++;
    }
}

void disp_name4() {

    dword color = 0;

    while (true) {

        while (Semaphore::down(&g_semaphore_console)) syscall_sleep(6);

        disp_write_font(78, 0, 'a', color%16);

        Semaphore::up(&g_semaphore_console);
        syscall_sleep(20);
        color++;
    }
}

void disp_process() {

    while (true) {
        dword eflags = get_eflags();
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 0, pos_y = 16;

        g_process_manager->printAllProcesses();

        pos_x = x;
        pos_y = y;
        set_eflags(eflags);
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

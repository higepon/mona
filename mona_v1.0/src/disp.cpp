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

    //    bool isOpen = SharedMemoryObject::open(0x1234, 4096 * 2);
    //    g_console->printf("shared memory is %s", isOpen ? "open" : "closed");

    //    bool isAttaced = SharedMemoryObject::attach(0x1234, g_current_process, 0xA0000000);
    //    g_console->printf("shared memory is %s", isAttaced ? "attaced" : "detached");

    while (true) {


        while (Semaphore::down(&g_semaphore_console)) syscall_sleep(8);

        disp_write_font(75, 0, 'M', color%13);

        Semaphore::up(&g_semaphore_console);
        syscall_sleep(40);

        //        dword* p = (dword*)0xA0000020;
        //        g_console->printf("value=**%x**", *p);

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
        syscall_sleep(50);
        color++;
    }
}

void disp_name3() {

    while (Semaphore::down(&g_semaphore_shared));
    bool isOpen = SharedMemoryObject::open(0x1234, 4096 * 2);
    bool isAttaced = SharedMemoryObject::attach(0x1234, g_current_process, 0x80000000);
    if (!isOpen || !isAttaced) panic("disp_name3");

    isOpen = SharedMemoryObject::open(0x1235, 4096 * 3);
    isAttaced = SharedMemoryObject::attach(0x1235, g_current_process, 0x90000000);
    if (!isOpen || !isAttaced) panic("disp_name3");

    isOpen = SharedMemoryObject::open(0x9999, 4096 * 3);
    isAttaced = SharedMemoryObject::attach(0x9999, g_current_process, 0xA0000000);
    if (!isOpen || !isAttaced) panic("disp_name3");
    Semaphore::up(&g_semaphore_shared);


    if (!isOpen || !isAttaced) panic("disp_name3");

    dword* p = (dword*)0x80000020;
    *p = 0x1234;
    dword* p2 = (dword*)0x90001020;
    *p2 = 0x5678;
    dword* p3 = (dword*)0xA0000020;
    *p3 = 0x4649;

    while (true) {

    }
}

void disp_name4() {


    while (Semaphore::down(&g_semaphore_shared));

    bool isOpen = SharedMemoryObject::open(0x1234, 4096 * 2);
    bool isAttaced = SharedMemoryObject::attach(0x1234, g_current_process, 0x90000000);
    if (!isOpen || !isAttaced) panic("disp_name4");

    isOpen = SharedMemoryObject::open(0x1235, 4096 * 3);
    isAttaced = SharedMemoryObject::attach(0x1235, g_current_process, 0x80000000);
    if (!isOpen || !isAttaced) panic("disp_name4");

    isOpen = SharedMemoryObject::open(0x9999, 4096 * 3);
    isAttaced = SharedMemoryObject::attach(0x9999, g_current_process, 0xA0000000);
    if (!isOpen || !isAttaced) panic("disp_name4");

    Semaphore::up(&g_semaphore_shared);


    dword* p = (dword*)0x90000020;
    g_console->printf("value=%x", *p);
    dword* p2 = (dword*)0x80001020;
    g_console->printf("value=%x", *p2);
    dword* p3 = (dword*)0xA0000020;
    g_console->printf("value=%x", *p3);

    while (true) {
    }
}

void disp_process() {

    while (true) {
        dword eflags = get_eflags();
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 0, pos_y = 15;

        //        g_process_manager->printAllProcesses();

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

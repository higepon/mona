/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002,2003 Higepon
    Copyright (c) 2002,2003 Guripon
    Copyright (c) 2003      .mjt

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both
    the above copyright notice(s) and this permission notice appear in supporting documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS
    NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
    WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written authorization of the copyright holder.
*/

/*!
    \file   kernel.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#define GLOBAL_VALUE_DEFINED

#include <types.h>
#include <global.h>

#include <kernel.h>
#include <operator.h>
#include <tester.h>
#include <checker.h>
#include <KeyBoardManager.h>
#include <SystemInfo.h>
#include <FDCDriver.h>
#include <GraphicalConsole.h>
#include <ihandlers.h>
#include <pic.h>
#include <rtc.h>
#include <disp.h>
#include <BitMap.h>
#include <FAT12.h>
#include <IA32MemoryManager.h>
#include <string.h>
#include <syscalls.h>
#include <ProcessManager.h>
#include <Message.h>
#include <MessageServer.h>
#include <syscalls.h>
#include <userlib.h>
#include <PageManager.h>

char* version = "Mona version 0.1.0 $Date$";
void userTest();
void userTest2();
void v86Test();

//static byte user_func[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x4D, 0x6F, 0x6E, 0x61, 0x21, 0x21, 0x21, 0x00, 0x55, 0x89, 0xE5, 0x83, 0xEC, 0x08, 0xC7, 0x04, 0x24, 0x00, 0x12, 0x00, 0x00, 0xE8, 0x3E, 0x00, 0x00, 0x00, 0xEB, 0xFE, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x55, 0x89, 0xE5, 0x83, 0xEC, 0x04, 0xBB, 0x05, 0x00, 0x00, 0x00, 0x8B, 0x75, 0x08, 0xCD, 0x80, 0x89, 0x45, 0xFC, 0x8B, 0x45, 0xFC, 0xC9, 0xC3, 0x55, 0x89, 0xE5, 0x83, 0xEC, 0x04, 0xBB, 0x06, 0x00, 0x00, 0x00, 0xCD, 0x80, 0x89, 0x45, 0xFC, 0x8B, 0x45, 0xFC, 0xC9, 0xC3, 0x90, 0x55, 0x89, 0xE5, 0x83, 0xEC, 0x04, 0xBB, 0x07, 0x00, 0x00, 0x00, 0x8B, 0x75, 0x08, 0xCD, 0x80, 0x89, 0x45, 0xFC, 0x8B, 0x45, 0xFC, 0xC9, 0xC3, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};

static byte user_func_from[] = {77,111,110,97,33,33,33,95,40,32,59,39,91,93,96,41,95,0,77,111,110,97,33,33,33,124,40,32,59,39,60,62,96,41,47,0,85,137,229,131,236,8,144,199,4,36,0,0,32,0,232,87,0,0,0,199,4,36,50,0,0,0,232,29,0,0,0,199,4,36,18,0,32,0,232,63,0,0,0,235,218,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,85,137,229,131,236,4,187,5,0,0,0,139,117,8,205,128,137,69,252,139,69,252,201,195,85,137,229,131,236,4,187,6,0,0,0,205,128,137,69,252,139,69,252,201,195,144,85,137,229,131,236,4,187,7,0,0,0,139,117,8,205,128,137,69,252,139,69,252,201,195,144,144,144,144,144,144,144,144,144,144,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static byte v86_func[] = {0xEB, 0xFE}; //loop forever

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:2003/06/08
*/
void startKernel(void) {

    /* set segment */
    GDTUtil::setup();

    /* Process setup */
    Process::setup();

    /* initialze console */
    g_console = new GraphicalConsole();

    /* show start message */
    printBanner();

    pic_init();

    printOK("Setting PIC        ");

    IDTUtil::setup();

    printOK("Setting IDT        ");
    printOK("Setting GDT        ");

    checkTypeSize();
    printOK("Checking type size ");

    g_total_system_memory = IA32MemoryManager::getTotalMemory();
    g_console->printf("\nSystem TotalL Memory %d[MB]\n", g_total_system_memory);

    /* paging start */
    g_page_manager = new PageManager(g_total_system_memory * 1024 * 1024);
    g_page_manager->setup();

    /* paging test */
    dword* p = (dword*)0x3FFFFE;
    *p = 5;
    p = (dword*)0xffffffff5;
    *p = 5;
    p = (dword*)0x4FFFFF;
    *p = 5;
    //    for (dword k = 0x3FFFFE; k < 0xffffffff; k++) {
    //
    //        p = (dword*)k;
    //        *p = 5;
    //    }

    g_console->printf("address of v86 = %x", (dword)v86_func);

    //     SystemInfo::rdtscsub();
//     g_console->printf("time=%x %x\n", SystemInfo::timeH, SystemInfo::timeL);
//     g_console->printf("userTestAddress=%x\n", (dword)userTest);

    Message* msg = new Message();
    msg->getNext();

#ifdef MJT
    test_mjt();
#endif

    g_console->printf("Hit any key to start [Process]\n");
    disableTimer();
    enableKeyboard();
    enableInterrupt();

    while (g_demo_step < 2);

#ifdef HIGE

    IA32MemoryManager& mm = IA32MemoryManager::instance();
    g_console->printf("used %d / total %d \n", mm.getUsedMemory(), mm.getTotalKernelMemory());

//     FDCTester();
//     g_console->printf("Hit any key to start [User/Kernel Process test]\n");
//     while (g_demo_step < 5);

    byte* user_func = (byte*)0x200000;

    g_console->printf("sizeof user_func_from %d" ,sizeof(user_func_from));

    memcpy(user_func, user_func_from, sizeof(user_func_from));

    g_info_level = MSG;

    Process* idle = new Process("idle     ");

    g_process_manager = new ProcessManager(idle);


    UserProcess* process1 = new UserProcess("user_process ");
    Process*     process2 = new Process("krnl_o       ");
    Process*     process3 = new Process("krnl_n       ");
    Process*     process4 = new Process("krnl_m       ");
    Process*     process5 = new Process("krnl_o2      ");
    UserProcess* process6 = new UserProcess("user_process2");
    Process*     process7 = new Process("show_process ");
    Process*     process8 = new Process("MessageServer");
    V86Process*  process9 = new V86Process("V86_process");

    g_process_manager->addProcess((Process*)process1, (virtual_addr)(user_func));
    g_process_manager->addProcess(process2          , (virtual_addr)disp_name2);
    g_process_manager->addProcess((Process*)process6, (virtual_addr)userTest2);
    g_process_manager->addProcess(process3          , (virtual_addr)disp_name3);
    g_process_manager->addProcess(process4          , (virtual_addr)disp_name1);
    g_process_manager->addProcess(process5          , (virtual_addr)disp_name4);
    g_process_manager->addProcess(process7          , (virtual_addr)disp_process);
    g_process_manager->addProcess(process8          , (virtual_addr)servermanager);
    //    g_process_manager->addProcess((Process*)process9, (virtual_addr)v86_func);
    //    process9->pinfo_.esp = 0x1000;

    enableTimer();

#endif

    while (g_demo_step < 5);
#ifndef MJT
    disableInterrupt();
#endif
    while (true);
}


void userTest() {

    while (true) {

        int result = syscall_heavy();
        while(true);
    }
}

void userTest2() {
    while (true) {

        syscall_sleep(500);
        //        syscall_print("Hello");
        syscall_heavy();
        while (true);
    }
}

void idle_process() {

    while (true) {

        /* do nothing */
    }
}

/*!
    \brief  mona kernel panic

    kernel panic

    \author HigePon
    \date   create:2002/12/02 update:2003/03/01
*/
void panic(const char* msg) {

    g_console->setCHColor(GP_RED);
    g_console->printf("kernel panic!!!!!\n%s", msg);
    while (true) {
    }
}

/*!
    \brief print OK

    print "msg             [OK]"

    \param msg message
    \author HigePon
    \date   create:2003/01/26 update:2003/01/25
*/
inline void printOK(const char* msg) {

    static int i = 0;

    if (i % 2) g_console->printf("   ");

    g_console->printf((char*)msg);
    g_console->printf("[");
    g_console->setCHColor(GP_LIGHTBLUE);
    g_console->printf("OK");
    g_console->setCHColor(GP_WHITE);
    g_console->printf("]");

    if (i % 2) g_console->printf("\n");
    i++;
}

/*!
    \brief print Banner

    print Banner

    \author HigePon
    \date   create:2003/01/26 update:2003/01/25
*/
inline void printBanner() {

    g_console->printf("------------------------------------------------------\n");
    g_console->printf("   Thanks for choosing MONA!                          \n");
    g_console->printf("            /\x18__/\x18                                    \n");
    g_console->printf("           ( ;'[]`)  < Ž³ŽÜŽÜŽÜŽ§ŽÝ!!                       \n");
    g_console->printf("           (      )                                   \n");
    g_console->printf("------------------------------------------------------\n");
    g_console->printf("%s\n\n", version);
    g_console->setCHColor(GP_WHITE);
}

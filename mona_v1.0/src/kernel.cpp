/*! \mainpage Mona Project source document
 *  &copy; Higepon Japan
 */

/*!
    \file   kernel.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.
    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

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
#include <PagingUtil.h>
#include <syscalls.h>
#include <ProcessManager.h>
#include <Message.h>
#include <MessageServer.h>

char* version = "Mona version 0.1.0 $Date$";
void userTest();
void userTest2();
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

    /* get System Information */
    SystemInfo& si = SystemInfo::instance();
    if (si.hasCpuid()) {

        printOK("Checking CPUID     ");
        si.printCpuid(g_console);
    } else {
        g_console->printf("CPUID NG  \n");
    }

    SystemInfo::rdtsc();
    g_console->printf("time=%x %x\n", SystemInfo::timeH, SystemInfo::timeL);

    g_total_system_memory = IA32MemoryManager::getTotalMemory();
    g_console->printf("System TotalL Memory %d[MB]\n", g_total_system_memory);

    /* now paging is off. */
    //   PagingUtil::setup();
    //   dword* p = (dword*)0x3FFFFC;
    //   dword* p = (dword*)0x3FFFFE;
    //   *p = 5;



    SystemInfo::rdtscsub();
    g_console->printf("time=%x %x\n", SystemInfo::timeH, SystemInfo::timeL);
    g_console->printf("userTestAddress=%x\n", (dword)userTest);

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

    g_info_level = WARNING;

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

            g_process_manager->addProcess((Process*)process1, (virtual_addr)userTest);
    //   g_process_manager->addProcess(process2          , (virtual_addr)disp_name2);
    //    g_process_manager->addProcess(process3          , (virtual_addr)disp_name3);
     //     g_process_manager->addProcess(process4          , (virtual_addr)disp_name1);
     //     g_process_manager->addProcess(process5          , (virtual_addr)disp_name4);
//     g_process_manager->addProcess(process7          , (virtual_addr)disp_process);
//     g_process_manager->addProcess((Process*)process6, (virtual_addr)userTest2);
//     g_process_manager->addProcess(process8          , (virtual_addr)servermanager);

    enableTimer();

#endif

    while (g_demo_step < 5);
#ifndef MJT
    disableInterrupt();
#endif
    while (true);
}


void userTest() {

    // you can't use hlt here
    //    asm volatile("hlt");
    while (true) {

        syscall_sleep(9);
    }
}

void userTest2() {
    while (true) {
        syscall_heavy();
        syscall_sleep(36);
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

    g_console->printf((char*)msg);
    g_console->printf("[");
    g_console->setCHColor(GP_RED);
    g_console->printf("OK");
    g_console->setCHColor(GP_WHITE);
    g_console->printf("]\n");
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

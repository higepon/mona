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
#include<global.h>

#include<kernel.h>
#include<idt.h>
#include<IA32MemoryManager.h>
#include<operator.h>
#include<tester.h>
#include<checker.h>
#include<KeyBoardManager.h>
#include<SystemInfo.h>
#include<FDCDriver.h>
#include<GraphicalConsole.h>
#include<ProcessManager.h>
#include<kthread.h>
#include<ihandlers.h>
#include<pic.h>
#include<rtc.h>
#include<BitMap.h>
#include<FAT12.h>
#include<string.h>

#include <IDEDriver.h> //in test...
char* version = "Mona develop beta 0.07a $Date$";

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:2003/02/22
*/
void startKernel(void) {

    /* re-set up GDT */
    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.resetGDT();

    /* initialze console */
    g_console = new GraphicalConsole();

    /* show start message */
    printBanner();

    pic_init();

    printOK("Setting PIC        ");

    /* set interrupt */
    _sysSetIdt();
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

    g_console->printf("BitMap:");
    BitMap* map = new BitMap(10);
    map->find();
    map->mark(5);
    map->mark(7);
    map->mark(8);
    for (int i = 0; i < 10; i++) {
        g_console->printf("%c", map->marked(i) ? 'x' : 'o');
    }
    g_console->printf("\n");

    /* set process name for info() */
    strcpy(g_process_name, "KERNEL");

#ifdef MJT
    test_mjt();
#endif

    g_console->printf("Hit any key to start [floppy read/write test]\n");
    disableTimer();
    enableKeyboard();
    enableInterrupt();

    /* set process name for info() */
    strcpy(g_process_name, "FDCDriver");
    while (g_demo_step < 2);

#ifdef HIGE
    FDCTester();
#endif

    while (g_demo_step < 5);
#ifndef MJT
    disableInterrupt();
#endif
    kthread_init();
    while (true);

    /* test code is here */
#if 0
    operatorTester();
    hvectorTester();
    typeTester();
#endif
    //    process2Tester();
    processTester();
    while (true) {
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

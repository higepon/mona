/*! \mainpage Mona Project source document
 *  &copy; Higepon Japan
 */

/*!
    \file   monaKernel.cpp
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

#include<monaKernel.h>
#include<idt.h>
#include<IA32MemoryManager.h>
#include<operator.h>
#include<tester.h>
#include<checker.h>
#include<KeyBoardManager.h>
#include<SystemInfo.h>
#include<MFDCDriver.h>
#include<GraphicalConsole.h>
#include<ProcessManager.h>
#include<kthread.h>
#include<monaIhandler.h>
#include<pic.h>
#include<rtc.h>

char* version = "Mona develop beta 0.05a $Date$";

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


    g_console->printf("Hit any key to start [floppy read/write test]\n");
    disableTimer();
    enableKeyboard();
    enableInterrupt();
    while (g_demo_step < 2);
    g_fdcdriver = new MFDCDriver(g_console);

    g_console->printf("\nHit any key to start [kernel thread demo]\n");
    while (g_demo_step < 5);
    disableInterrupt();
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
    g_console->setCHColor(GP_SKYBLUE);
    g_console->printf("      Mona Kernel starting                            \n");
    g_console->setCHColor(GP_WHITE);
    g_console->printf("        ________ A A                                  \n");
    g_console->printf("      ~/ ______( ");
    g_console->setCHColor(GP_RED);
    g_console->printf("`");
    g_console->setCHColor(GP_WHITE);
    g_console->printf("D");
    g_console->setCHColor(GP_RED);
    g_console->printf("`");
    g_console->setCHColor(GP_WHITE);
    g_console->printf(") < ");
    g_console->setCHColor(GP_SKYBLUE);
    g_console->printf("ŽÓŽÅ\n");
    g_console->setCHColor(GP_WHITE);
    g_console->printf("        UU       U U                                  \n");
    g_console->printf("------------------------------------------------------\n");
    g_console->setCHColor(GP_LIGHTGREEN);
    g_console->printf("%s\n\n", version);
    g_console->setCHColor(GP_WHITE);
}

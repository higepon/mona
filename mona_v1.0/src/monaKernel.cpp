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

#include<monaKernel.h>
#include<monaIdt.h>
#include<IA32MemoryManager.h>
#include<monaOperator.h>
#include<monaTester.h>
#include<monaChecker.h>
#include<KeyBoardManager.h>
#include<SystemInfo.h>
#include<MFDCDriver.h>
#include<GraphicalConsole.h>
#include<ProcessManager.h>
#include<kthread.h>
#include<monaIhandler.h>
#include<pic.h>
#include<rtc.h>

char* version = "Mona develop beta 0.04a $Date$";

VirtualConsole* console;
dword demoStep;

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
    console = new GraphicalConsole();

    /* show start message */
    printBanner();

    pic_init();
    enableKeyboard();
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
        si.printCpuid(console);
    } else {
        console->printf("CPUID NG  \n");
    }

//      Date date;
//      rtc_get_date(&date);
//      console->printf("%d %d/%d %d:%d %d\n", (dword)(date.year), (dword)(date.month), (dword)(date.day), (dword)(date.hour), (dword)(date.min), (dword)(date.sec));

    /* enable interrupt */
    enableInterrupt();

    kthread_init();
    enableTimer();
    while (true);

    /* test code is here */
#if 0
    operatorTester();
    hvectorTester();
    typeTester();
    STLTester();
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

    console->setCHColor(GP_RED);
    console->printf("kernel panic!!!!!\n%s", msg);
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

    console->printf((char*)msg);
    console->printf("[");
    console->setCHColor(GP_RED);
    console->printf("OK");
    console->setCHColor(GP_GRAY);
    console->printf("]\n");
}

/*!
    \brief print Banner

    print Banner

    \author HigePon
    \date   create:2003/01/26 update:2003/01/25
*/
inline void printBanner() {

    console->printf("------------------------------------------------------\n");
    console->setCHColor(GP_SKYBLUE);
    console->printf("      Mona Kernel starting                            \n");
    console->setCHColor(GP_GRAY);
    console->printf("        ________ A A                                  \n");
    console->printf("      ~/ ______( ");
    console->setCHColor(GP_RED);
    console->printf("`");
    console->setCHColor(GP_GRAY);
    console->printf("D");
    console->setCHColor(GP_RED);
    console->printf("`");
    console->setCHColor(GP_GRAY);
    console->printf(") < ");
    console->setCHColor(GP_SKYBLUE);
    console->printf("ŽÓŽÅ\n");
    console->setCHColor(GP_GRAY);
    console->printf("        UU       U U                                  \n");
    console->printf("------------------------------------------------------\n");
    console->setCHColor(GP_LIGHTGREEN);
    console->printf("%s\n\n", version);
    console->setCHColor(GP_GRAY);
}

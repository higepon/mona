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
#include<monaIhandler.h>

char* version = "Mona develop beta 0.04 $Date$";

VirtualConsole* console;
Process* current;

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

    /* set interrept */
    _sysSetIdt();
    _sysInitIo();
    printOK("Setting IDT        ");
    disableTimer();

    /* enable interrupt */
    enableInterrupt();

    Process process;
    current = &process;

    enableTimer();
    printOK("Setting GDT        ");

    /* check some */
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

    /* set up KeyBoardManager before task start */
    KeyBoardManager::instance();

    gMFDCDriver = new MFDCDriver(console);
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
    \date   create:2002/12/02 update:2003/01/25
*/
void panic(const char* msg) {

    _sysSetColor(SYS_BG_COLOR | CH_RED);
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
    _sysSetColor(SYS_BG_COLOR | CH_RED);
    console->printf("OK");
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
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
    _sysSetColor(SYS_BG_COLOR | CH_AQUA);
    console->printf("      Mona Kernel starting                            \n");
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    console->printf("        ________ A A                                  \n");
    console->printf("      ~/ ______( ");
    _sysSetColor(SYS_BG_COLOR | CH_FUCHSIA);
    console->printf("`");
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    console->printf("D");
    _sysSetColor(SYS_BG_COLOR | CH_FUCHSIA);
    console->printf("`");
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    console->printf(") < thanks ProgrammingBoard@2Á¬Ý  \n");
    console->printf("        UU       U U                                  \n");
    console->printf("------------------------------------------------------\n");
    _sysSetColor(SYS_BG_COLOR | CH_MAROON);
    console->printf("%s\n", version);
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    console->printf("\nÓÅ Ä Ò¸Þ Ê ·®³ÀÞ²ÃÞ½\n\n");
}

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
#include<monaVga.h>
#include<monaIdt.h>
#include<monaIo.h>
#include<IA32MemoryManager.h>
#include<monaOperator.h>
#include<FDCDriver.h>
#include<monaTester.h>
#include<monaChecker.h>
#include<KeyBoardManager.h>
#include<SystemInfo.h>

char* version = "Mona develop beta 0.03a $Date$";

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:$Date$
*/
void startKernel(void) {

    /* initialize screen */
    _sysInitVga();
    _sysClearScreen();

    /* show message */
    _sysPrintln("------------------------------------------------------");
    _sysSetColor(SYS_BG_COLOR | CH_AQUA);
    _sysPrintln("      Mona Kernel starting                            ");
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    _sysPrintln("        ________ A A                                  ");
    _sysPrintln("      ~/ ______( `D`) < thanks ProgrammingBoard@2ch   ");
    _sysPrintln("        UU       U U                                  ");
    _sysPrintln("------------------------------------------------------");
    _sysSetColor(SYS_BG_COLOR | CH_MAROON);
    _sys_printf("%s\n", version);
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);

    /* set interrept */
    _sysSetIdt();
    _sysInitIo();
    printOK("Setting IDT        ");
    disableTimer();

    /* re-set up GDT */
    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.resetGDT();

    /* enable interrupt */
    _sysUnlock();
    printOK("Setting GDT        ");

    /* check some */
    checkTypeSize();
    printOK("Checking type size ");


    /* get System Information */
    SystemInfo& si = SystemInfo::instance();
    if (si.hasCpuid()) {

        printOK("Checking CPUID     ");
        si.cpuid();
    } else {
        _sys_printf("CPUID NG  \n");
    }

    /* set up KeyBoardManager before task start */
    KeyBoardManager::instance();

    /* test code is here */
#if 0
    operatorTester();
    hvectorTester();
    floppyTester();
    typeTester();
#endif
    STLTester();
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
    _sys_printf("kernel panic!!!!!\n%s", msg);
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
void printOK(const char* msg) {

    _sys_printf((char*)msg);
    _sys_printf("[");
    _sysSetColor(SYS_BG_COLOR | CH_RED);
    _sys_printf("OK");
    _sysSetColor(SYS_BG_COLOR | SYS_CH_COLOR);
    _sys_printf("]\n");
}

/*! \mainpage Mona Project source document
 *  &copy; Higepon Japan
 */

/*!
    \file   monaKernel.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.
    Copyright (c) 2002 HigePon
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
    _sysSetColor(BG_BLACK | CH_AQUA);
    _sysPrintln("      Mona Kernel starting                         ");
    _sysSetColor(BG_BLACK | CH_SILVER);
    _sysPrintln("        ________ A A                                  ");
    _sysPrintln("      ~/ ______( `D`) < thanks ProgrammingBoard@2ch   ");
    _sysPrintln("        UU       U U                                  ");
    _sysPrintln("------------------------------------------------------");

    _sysSetColor(BG_MAROON | CH_BLUE);
    _sys_printf("\nMona develop beta 0.01a\n\n");
    _sysSetColor(BG_BLACK | CH_SILVER);

    /* set interrept */
    _sysSetIdt();
    _sysInitIo();

    /* re-set up GDT */
    IA32MemoryManager& mm = IA32MemoryManager::instance();
    mm.resetGDT();
    _sysUnlock();
    _sys_printf("IDT,GDT set done\n");

    /* check some */
    checkTypeSize();
    _sys_printf("Check type size done\n");

    /* enable A20 */
    //    IA32MemoryManager::enableA20();

    /* set up KeyBoardManager before task start */
    KeyBoardManager::instance();

    /* test code is here */
#if 0
    operatorTester();
    hvectorTester();
    floppyTester();
    typeTester();
#endif
    processTester();

    while (true) {
    }

}

/*!
    \brief  mona kernel panic

    kernel panic

    \author HigePon
    \date   create:2002/12/02 update:$Date$
*/
void panic(const char* msg) {

    _sysSetColor(BG_BLACK | CH_RED);
    _sys_printf("kernel panic!!!!!\n%s", msg);
    while (true) {
    }
}

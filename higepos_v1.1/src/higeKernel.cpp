/*!
    \file   higeKernel.cpp
    \brief  higepos kernel start at this point

    higepos kernel start at this point.
    before startKernel, os entered protected mode.
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/
#include<higeKernel.h>
#include<higeVga.h>
#include<higeIdt.h>
#include<higeIo.h>
#include<X86MemoryManager.h>
#include<higeOperator.h>
#include<FDCDriver.h>
#include<higeTester.h>

/*!
    \brief  higepos kernel start at this point

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
    _sysSetColor(BG_BLACK | CH_TEAL);
    _sysPrintln("      Higepos Kernel starting                         ");
    _sysSetColor(BG_BLACK | CH_SILVER);
    _sysPrintln("        ________ A A                                  ");
    _sysPrintln("      ~/ ______( `D`) < thanks ProgrammingBoard@2ch   ");
    _sysPrintln("        UU       U U                                  ");
    _sysPrintln("------------------------------------------------------");

    /* set interrept */
    _sysSetIdt();
    _sysInitIo();
    _sysUnlock();
    _sys_printf("idt set done\n");

    /* enable A20 */
    X86MemoryManager::enableA20();

    /* test code is here */
#if 0
    operatorTester();
    hvectorTester();
    floppyTester();
#endif


    while (true) {
    }
}


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
    _sysPrintln("      Higepos Kernel starting                         ");
    _sysPrintln("        ________ A A                                  ");
    _sysPrintln("      ~/ ______( `D`) < thanks ProgrammingBoard@2ch   ");
    _sysPrintln("        UU       U U                                  ");
    _sysPrintln("------------------------------------------------------");

    /* set interrept */
    _sysSetIdt();
    _sysInitIo();
    _sysUnlock();
    _sys_printf("idt set done\n");

    /* test code is here */
#if 0
    operatorTester();
    hvectorTester();
#endif

#if 0
    /* FDCDriver test code */
    unsigned char buff[512];
    gFDCDriver1 = new FDCDriver(0);
    gFDCDriver1->readSector(1, 1, buff);
    for(int i = 0; i < 512; i++){
         _sys_printf("[%d]", buff[i]);
    }
#endif

    while (true) {
    }
}


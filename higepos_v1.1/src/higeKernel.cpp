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
#include<higeUtil.h>
#include<higeTypes.h>

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
    _sysPrintln("idt set done");

    /* testing types */
    _sys_printf("[sizeof(H_SIZE_T) is %d byte]  ", sizeof(H_SIZE_T));
    _sys_printf("[sizeof(H_BYTE) is %d byte]\n", sizeof(H_BYTE));


    /* testing operator new */
    Point* point1 = new Point();
    Point* point2 = new Point(6, -2);
    Point* point3 = new Point(7, -100);
    delete(point3);

    _sys_printf("[Point() getY() = %d]\n", point1->getY());
    _sys_printf("[Point(6, -2) getY() = %d]\n", point2->getY());
    _sys_printf("[Point(7, -100) getY() = %d]\n", point3->getY());
    Point* point4 = new Point(7, -100);

    while (true) {
    }
}

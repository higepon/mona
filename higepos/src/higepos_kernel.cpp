/*!
    \file   higepos_kernel.cpp
    \brief  higepos kernel start at this point

    higepos kernel start at this point.
    before startKernel, os entered protected mode.
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/
#include<kernel.h>
#include<vga.h>
#include<idt.h>
#include<io.h>
#include<X86MemoryManager.h>
#include<higeOperator.h>
#include<higeUtil.h>
#include<Vector.h>
#include<higepostypes.h>

/*!
    \brief  higepos kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:$Date$
*/
void startKernel(void) {
    _sysInitVga();
    _sysClearScreen();
    _sysPrintln("------------------------------------------");
    _sysPrintln("      Higepos Kernel starting             ");
    _sysPrintln("        ________ A A                      ");
    _sysPrintln("      ~/ ______( `D`) < Powerd by 2ch     ");
    _sysPrintln("        UU       U U                      ");
    _sysPrintln("------------------------------------------");


    _sysSetIdt();
    _sysInitIo();
    _sysUnlock();
    _sysPrintln("idt set done");

    /* testing operator new */
    //    Point* point1 = new Point();?????????
    //    _sysPrintlnInt(point1->getX());
    Point* point2 = new Point(5, -1);
    _sys_printf("[Point(5, -1) getY() = %d]\n", point2->getY());

    /* testing types */
    _sys_printf("[sizeof(H_SIZE_T) is %d byte]  ", sizeof(H_SIZE_T));
    _sys_printf("[sizeof(H_BYTE) is %d byte]\n", sizeof(H_BYTE));

    /* testing malloc */
    _sys_printf("address of (int)malloc(sizeof(int)) is %d \n", (int)malloc(sizeof(int)));
    _sys_printf("address of (int)malloc(sizeof(int)) is %d \n", (int)malloc(sizeof(int)));

    /* testing vector */
//      Vector<int> list;
//      list.add(10);
//      list.add(10);
//      _sysPrintlnInt(list.get(0));

    while (true) {
    }
}

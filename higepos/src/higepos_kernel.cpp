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
    \date   create:2002/07/21 update:2002/08/23
*/
void startKernel(void) {
    _sysInitVga();
    _sysClearScreen();
    _sysPrintln("------------------------------------------");
    _sysPrintln("      Higepos Kernel starting             ");
    _sysPrintln("      Powered by 2ch                      ");
    _sysPrintln("------------------------------------------");

    _sysSetIdt();
    _sysInitIo();
    _sysUnlock();
    _sysPrintln("idt set done");

    /* testing operator new */
    //    Point* point1 = new Point();?????????
    //    _sysPrintlnInt(point1->getX());
    Point* point2 = new Point(5, 5);
    _sysPrintlnInt(point2->getX());
    Point* point3 = new Point(6, 6);
    _sysPrintlnInt(point3->getX());

    /* testing types */
    _sysPrintlnInt(sizeof(H_SIZE_T));
    _sysPrintlnInt(sizeof(H_BYTE));


    /* testing malloc */
    _sysPrintlnInt((int)malloc(sizeof(int)));
    _sysPrintlnInt((int)malloc(sizeof(int)));

    /* testing _sys_printf */
    _sysPrintln("printf start");
    _sys_printf("1+4 = %d %s is the%d\n", 5, "hige",10);
    _sys_printf("1+4 = %d this is the%d\n", 5, -1);
    _sys_printf("%d %s %s %d", (int)5,"can you see me?", "yes", (int)-1);
    /* testing vector */
//      Vector<int> list;
//      list.add(10);
//      list.add(10);
//      _sysPrintlnInt(list.get(0));

    while (true) {
    }
}

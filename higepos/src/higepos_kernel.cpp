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
    Point* point = new Point();
    _sysPrintlnInt(point->getX());
    point = new Point(5, 5);
    _sysPrintlnInt(point->getX());

    /* testing malloc */
    _sysPrintlnInt((int)malloc(sizeof(int)));
    _sysPrintlnInt((int)malloc(sizeof(int)));
    while (true) {
    }
}

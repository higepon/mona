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

/*!
    \brief  higepos kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:2002/07/27
*/
void startKernel(void) {
    _sysInitVga();
    _sysClearScreen();
    _sysPrint("**************************************\n");
    _sysPrint("Higepos Kernel start!!\nPowered by 2ch\n");
    _sysPrint("**************************************\n");
    _sysSetIdt();
    _sysInitIo();
    _sysUnlock();
    _sysPrint("idt set done!");
    while (true) {
    }
}


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
#include<HVector.h>
#include<FDCDriver.h>
#include<KeyBoardManager.h>

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

#if 0
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

    /* testing HVector */
    _sys_printf("HVector test start\n");
    HVector<char*>* v = new HVector<char*>(3, 5);
    v->add("one");
    v->add("two");
    v->add("three");
    v->add("four");
    v->add("five");
    v->add("six");
    v->add("seven");
    _sys_printf("element %s removed\n", v->remove(0));
    _sys_printf("element %s removed\n", v->remove(4));
    for (int j = 0; j < v->size(); j++) {
        _sys_printf("(%d, %s) ", j, v->get(j));
    }
    delete(v);

    /* testing keyboardmanager */
    KeyBoardManager& km = KeyBoardManager::instance();
    km.printInfo();

    while (true) {
    }
}


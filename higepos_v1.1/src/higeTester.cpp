/*!
    \file   higeTester.cpp
    \brief  test code

    test code at kernel should be placed here.

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/11 update:$$
*/
#include<higeVga.h>
#include<higeIo.h>
#include<higeOperator.h>
#include<higeUtil.h>
#include<higeTypes.h>
#include<HVector.h>
#include<higeTester.h>
#include<FDCDriver.h>

/*!
    \brief test code for HVector

    test code for HVector

    \author HigePon
    \date   create:2002/11/11 update:$Date$
*/
void hvectorTester() {

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
    _sys_printf("\n");
    delete(v);
}

/*!
    \brief test code for operator new

    test code for operator new

    \author HigePon
    \date   create:2002/11/11 update:$Date$
*/
void operatorTester() {

    Point* point1 = new Point();
    Point* point2 = new Point(6, -2);
    Point* point3 = new Point(7, -100);
    delete(point3);
    _sys_printf("[Point() getY() = %d]\n", point1->getY());
    _sys_printf("[Point(6, -2) getY() = %d]\n", point2->getY());
    _sys_printf("[Point(7, -100) getY() = %d]\n", point3->getY());
    Point* point4 = new Point(7, -100);
}

/*!
    \brief test code for floppy disk (class FDCDriver)

    test code for floppy disk (class FDCDriver)
    
    \author syn
    \date create:2002/11/14 update:2002/11/14
*/
void floppyTester(){
    unsigned char buff[512];
    gFDCDriver1 = new FDCDriver(0);
    gFDCDriver1->readSector(1, 1, buff);
    for(int i = 0; i < 512; i++){
         _sys_printf("[%d]", buff[i]);
    }
}


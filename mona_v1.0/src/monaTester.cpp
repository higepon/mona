/*!
    \file   monaTester.cpp
    \brief  test code

    test code at kernel should be placed here.

    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/11 update:$Date$
*/
#include<monaVga.h>
#include<monaIo.h>
#include<monaIdt.h>
#include<monaOperator.h>
#include<monaUtil.h>
#include<monaTypes.h>
#include<HVector.h>
#include<monaTester.h>
#include<FDCDriver.h>
#include<monaTypes.h>
#include<ProcessManager.h>

#include<string>
#include<list>
#include<vector>
#include<algorithm>


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
    for (size_t j = 0; j < v->size(); j++) {
        _sys_printf("(%d, %s) ", j, v->get(j));
    }
    _sys_printf("\n");
    delete(v);
}

/*!
    \brief test code for operator new

    test code for operator new

    \author HigePon
    \date   create:2002/11/11 update:$2003/01/11$
*/
void operatorTester() {

    Point* point1 = new Point();
    Point* point2 = new Point(6, -2);
    Point* point3 = new Point(7, -100);
    delete(point3);
    _sys_printf("[Point() getY() = %d]\n", point1->getY());
    _sys_printf("[Point(6, -2) getY() = %d]\n", point2->getY());
    _sys_printf("[Point(7, -100) getY() = %d]\n", point3->getY());
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

/*!
    \brief test code for monaTypes

    test code for monaTypes

    \author HigePon
    \date   create:2002/11/20 update:$2003/01/11$
*/
void typeTester() {

    _sys_printf("size of byte =%d\n", sizeof(byte));
    _sys_printf("size of word =%d\n", sizeof(word));
    _sys_printf("size of dword=%d\n", sizeof(dword));
}

/*!
    \brief test code for process manager

    test code for process manager

    \author HigePon
    \date   create:2002/11/20 update:$Date$
*/
void processTester() {

    ProcessManager& pm = ProcessManager::instance();
    pm.multiTaskTester();
    return;
}


void process1Tester() {
    enableTimer();

    //    _sysdump("process1", true, true);
    while (true) {
         for (dword i = 0; i < 900000; i++) {
            i += 1;
            i -= 1;
        }
        _sysSetColor(BG_BLACK | CH_FUCHSIA);
        _sys_printf("  (.o'v'o)  ");
        _sysSetColor(BG_BLACK | CH_SILVER);
    }
    return;
}

void process2Tester() {
    _sysUnlock();

    //    _sysdumpReg("process2", true, false);
    while (true) {
        _sysSetColor(BG_BLACK | CH_YELLOW);
        _sys_printf("  ( '  v ')  ");
        _sysSetColor(BG_BLACK | CH_SILVER);
        for (dword i = 0; i < 900000; i++) {
            i += 1;
            i -= 1;
        }
    }
    return;
}

/*!
    \brief test code for STLport

    test code for STLport

    \author HigePon
    \date   create:2003/01/09 update:2003/01/11
*/
void STLTester() {

    _sys_printf("--------STL Test--------------------------------------------------------\n");

    // string
    std::string str = "---";
    str += "string";
    str += "@mona ";
    str += "is OK";
    str += "---";
    _sys_printf("string: %s\n", str.data());
    _sys_printf("string: str.substr(3, 11): %s\n", str.substr(3, 11).data());

    // list
    std::list<const char*> li;
    li.push_back("micoro kernel ");
    li.push_back("operating system with ");
    li.push_back("network suit architecture ");
    li.push_back("\n");
    std::list<const char*>::iterator it;
    _sys_printf("list  : iterate->");
    for (it = li.begin(); it != li.end(); it++) {
        _sys_printf("%s", *it);
    }

    // vector
    std::vector<const char*> ve;
    ve.push_back("1");
    ve.push_back("2");
    ve.push_back("3");
    ve.push_back("4");
    std::vector<const char*>::iterator it2;
    _sys_printf("vector : iterate->");
    for (it2 = ve.begin(); it2 != ve.end(); it2++) {
        _sys_printf("%s ", *it2);
    }

    // argorithm
    std::reverse(ve.begin(), ve.end());
    _sys_printf("\nargorithm : reverse->");
    for (it2 = ve.begin(); it2 != ve.end(); it2++) {
        _sys_printf("%s ", *it2);
    }

    std::fill(ve.begin(), ve.end(), (char*)"7");
    _sys_printf("\nargorithm : fill with \"7\"->");
    for (it2 = ve.begin(); it2 != ve.end(); it2++) {
        _sys_printf("%s ", *it2);
    }
    _sys_printf("\n--------STL Test END---------------------------------------------------\n");
}

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
#include<global.h>
#include<monaIdt.h>
#include<operator.h>
#include<monaTypes.h>
#include<HVector.h>
#include<monaTester.h>
#include<monaTypes.h>
#include<ProcessManager.h>
#include<KeyBoardManager.h>
#include<Semaphore.h>

/*!
    \brief test code for HVector

    test code for HVector

    \author HigePon
    \date   create:2002/11/11 update:2003/01/26
*/
void hvectorTester() {

    g_console->printf("HVector test start\n");
    HVector<char*>* v = new HVector<char*>(3, 5);
    v->add("one");
    v->add("two");
    v->add("three");
    v->add("four");
    v->add("five");
    v->add("six");
    v->add("seven");
    g_console->printf("element %s removed\n", v->remove(0));
    g_console->printf("element %s removed\n", v->remove(4));
    for (size_t j = 0; j < v->size(); j++) {
        g_console->printf("(%d, %s) ", j, v->get(j));
    }
    g_console->printf("\n");
    delete(v);
}


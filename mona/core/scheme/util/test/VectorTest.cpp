/*!
    \file   VectorTest.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "VectorTest.h"
#include "../Vector.h"
#include <vector>

CPPUNIT_TEST_SUITE_REGISTRATION(VectorTest);

using namespace util;
using namespace std;

void VectorTest::setUp()
{
}

void VectorTest::tearDown()
{
}

void VectorTest::testAddGetPointerStack()
{
    Vector<void*> v;
    vector<void*> sv;
    for (int i = 0; i < 10; i++)
    {
        void* p = malloc(i * 32);
        sv.push_back(p);
        v.add(p);
    }

    CPPUNIT_ASSERT(v.size() == 10);
    CPPUNIT_ASSERT(v.get(0) == sv[0]);
    CPPUNIT_ASSERT(v.get(1) == sv[1]);
    CPPUNIT_ASSERT(v.get(2) == sv[2]);
    CPPUNIT_ASSERT(v.get(3) == sv[3]);
    CPPUNIT_ASSERT(v.get(4) == sv[4]);
    CPPUNIT_ASSERT(v.get(5) == sv[5]);
    CPPUNIT_ASSERT(v.get(6) == sv[6]);
    CPPUNIT_ASSERT(v.get(7) == sv[7]);
    CPPUNIT_ASSERT(v.get(8) == sv[8]);
    CPPUNIT_ASSERT(v.get(9) == sv[9]);
    for (int i = 0; i < 10; i++)
    {
        void* p = sv[i];
        free(p);
    }
}

void VectorTest::testAddGetPointerHeap()
{
    Vector<void*>* v = new Vector<void*>;
    vector<void*> sv;
    for (int i = 0; i < 10; i++)
    {
        void* p = malloc(i * 32);
        sv.push_back(p);
        v->add(p);
    }

    CPPUNIT_ASSERT(v->size() == 10);
    CPPUNIT_ASSERT(v->get(0) == sv[0]);
    CPPUNIT_ASSERT(v->get(1) == sv[1]);
    CPPUNIT_ASSERT(v->get(2) == sv[2]);
    CPPUNIT_ASSERT(v->get(3) == sv[3]);
    CPPUNIT_ASSERT(v->get(4) == sv[4]);
    CPPUNIT_ASSERT(v->get(5) == sv[5]);
    CPPUNIT_ASSERT(v->get(6) == sv[6]);
    CPPUNIT_ASSERT(v->get(7) == sv[7]);
    CPPUNIT_ASSERT(v->get(8) == sv[8]);
    CPPUNIT_ASSERT(v->get(9) == sv[9]);
    delete v;
    for (int i = 0; i < 10; i++)
    {
        void* p = sv[i];
        free(p);
    }
}

void VectorTest::testAddGetIntHeap()
{
    Vector<int>* v = new Vector<int>;
    v->add(0);
    v->add(1);
    v->add(2);
    v->add(3);
    v->add(4);
    v->add(5);
    v->add(6);
    v->add(7);
    v->add(8);
    v->add(9);

    CPPUNIT_ASSERT(v->size() == 10);
    CPPUNIT_ASSERT(v->get(0) == 0);
    CPPUNIT_ASSERT(v->get(1) == 1);
    CPPUNIT_ASSERT(v->get(2) == 2);
    CPPUNIT_ASSERT(v->get(3) == 3);
    CPPUNIT_ASSERT(v->get(4) == 4);
    CPPUNIT_ASSERT(v->get(5) == 5);
    CPPUNIT_ASSERT(v->get(6) == 6);
    CPPUNIT_ASSERT(v->get(7) == 7);
    CPPUNIT_ASSERT(v->get(8) == 8);
    CPPUNIT_ASSERT(v->get(9) == 9);

    delete v;
}

void VectorTest::testAddGetIntStack()
{
    Vector<int> v;
    v.add(0);
    v.add(1);
    v.add(2);
    v.add(3);
    v.add(4);
    v.add(5);
    v.add(6);
    v.add(7);
    v.add(8);
    v.add(9);

    CPPUNIT_ASSERT(v.size() == 10);
    CPPUNIT_ASSERT(v.get(0) == 0);
    CPPUNIT_ASSERT(v.get(1) == 1);
    CPPUNIT_ASSERT(v.get(2) == 2);
    CPPUNIT_ASSERT(v.get(3) == 3);
    CPPUNIT_ASSERT(v.get(4) == 4);
    CPPUNIT_ASSERT(v.get(5) == 5);
    CPPUNIT_ASSERT(v.get(6) == 6);
    CPPUNIT_ASSERT(v.get(7) == 7);
    CPPUNIT_ASSERT(v.get(8) == 8);
    CPPUNIT_ASSERT(v.get(9) == 9);
}

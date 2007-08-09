/*!
    \file  VectorTest.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef _VECTOR_TEST_H_
#define _VECTOR_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

class VectorTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(VectorTest);
    CPPUNIT_TEST(testAddGetIntHeap);
    CPPUNIT_TEST(testAddGetIntStack);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();

    void testAddGetIntHeap();
    void testAddGetPointerHeap();
    void testAddGetIntStack();
    void testAddGetPointerStack();
};

#endif // _VECTOR_TEST_H_

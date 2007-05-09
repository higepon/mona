#ifndef _LINE_TEST_H
#define _LINE_TEST_H

#include "text/Line.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

class LineTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(LineTest);
    CPPUNIT_TEST(testAppend);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testCursor);
    CPPUNIT_TEST(testReset);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testAppend();
    void testInsert();
    void testCursor();
    void testReset();

protected:
};

#endif // _LINE_TEST_H

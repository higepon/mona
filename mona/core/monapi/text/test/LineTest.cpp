#include "LineTest.h"

using namespace MonAPI;
using namespace MonAPI::text;

CPPUNIT_TEST_SUITE_REGISTRATION(LineTest);

void LineTest::setUp()
{
}

void LineTest::tearDown()
{
}

void LineTest::testReset()
{
    Line line;
    line.write("ABC");
    line.reset();
    CPPUNIT_ASSERT_EQUAL(0, line.getCursorPosition());
    CPPUNIT_ASSERT_EQUAL(CString(""), line.get());
}

void LineTest::testAppend()
{
    Line line;
    line.write("ABC");
    CPPUNIT_ASSERT_EQUAL(3, line.getCursorPosition());
    line.write("DEF");
    CPPUNIT_ASSERT_EQUAL(6, line.getCursorPosition());
    CPPUNIT_ASSERT_EQUAL(CString("ABCDEF"), line.get());
}

void LineTest::testInsert()
{
    Line line;
    line.write("ABC");
    CPPUNIT_ASSERT_EQUAL(3, line.getCursorPosition());
    line.moveCursorLeft();
    CPPUNIT_ASSERT_EQUAL(2, line.getCursorPosition());
    line.write("DE");
    CPPUNIT_ASSERT_EQUAL(CString("ABDE"), line.get());
    CPPUNIT_ASSERT_EQUAL(4, line.getCursorPosition());
}

void LineTest::testCursor()
{
    Line line;
    line.write("ABC");
    CPPUNIT_ASSERT_EQUAL(3, line.getCursorPosition());
    line.moveCursorLeft();
    CPPUNIT_ASSERT_EQUAL(2, line.getCursorPosition());
    line.moveCursorLeft();
    CPPUNIT_ASSERT_EQUAL(1, line.getCursorPosition());
    line.moveCursorLeft();
    CPPUNIT_ASSERT_EQUAL(0, line.getCursorPosition());
    line.moveCursorLeft();
    CPPUNIT_ASSERT_EQUAL(0, line.getCursorPosition());
    line.moveCursorRight();
    CPPUNIT_ASSERT_EQUAL(1, line.getCursorPosition());
    line.write("D");
    CPPUNIT_ASSERT_EQUAL(2, line.getCursorPosition());
}

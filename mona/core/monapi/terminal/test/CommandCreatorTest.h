#ifndef _COMMAND_CREATOR_TEST_H
#define _COMMAND_CREATOR_TEST_H

#include "terminal/CommandCreator.h"
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

class CommandCreatorTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CommandCreatorTest);
    CPPUNIT_TEST(testDrawCursor);
    CPPUNIT_TEST(testEraseCursor);
    CPPUNIT_TEST(testClearScreen);
    CPPUNIT_TEST(testMoveCursor);
    CPPUNIT_TEST(testMoveCursorUp);
    CPPUNIT_TEST(testMoveCursorDown);
    CPPUNIT_TEST(testMoveCursorLeft);
    CPPUNIT_TEST(testMoveCursorRight);
    CPPUNIT_TEST(testMoveCursorTo);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testDrawCursor();
    void testEraseCursor();
    void testClearScreen();
    void testMoveCursor();
    void testMoveCursorTo();
    void testMoveCursorUp();
    void testMoveCursorDown();
    void testMoveCursorRight();
    void testMoveCursorLeft();

protected:
   MonAPI::terminal::CommandCreator* creator_;
};

#endif // _COMMAND_CREATOR_TEST_H

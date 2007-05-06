#ifndef _COMMAND_PARSER_TEST_H
#define _COMMAND_PARSER_TEST_H

#include <terminal/CommandCreator.h>
#include <terminal/CommandParser.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include "TestWriter.h"

class CommandParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(CommandParserTest);
    CPPUNIT_TEST(testDrawCursor);
    CPPUNIT_TEST(testEraseCursor);
    CPPUNIT_TEST(testClearScreen);
    CPPUNIT_TEST(testMoveCursor);
    CPPUNIT_TEST(testMoveCursorUp);
    CPPUNIT_TEST(testMoveCursorDown);
    CPPUNIT_TEST(testMoveCursorLeft);
    CPPUNIT_TEST(testMoveCursorRight);
    CPPUNIT_TEST(testMix);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testDrawCursor();
    void testEraseCursor();
    void testClearScreen();
    void testMoveCursor();
    void testMoveCursorUp();
    void testMoveCursorDown();
    void testMoveCursorRight();
    void testMoveCursorLeft();
    void testMix();

protected:
    MonAPI::terminal::CommandParser* parser_;
    MonAPI::terminal::CommandCreator* creator_;
    MonAPI::terminal::TestWriter* writer_;
};

#endif // _COMMAND_PARSER_TEST_H

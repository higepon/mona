#include "CommandParserTest.h"

using namespace MonAPI::terminal;

CPPUNIT_TEST_SUITE_REGISTRATION(CommandParserTest);

void CommandParserTest::setUp()
{
    writer_ = new TestWriter();
    parser_ = new CommandParser(writer_);
    creator_ = new CommandCreator();
}

void CommandParserTest::tearDown()
{
    delete creator_;
    delete parser_;
    delete writer_;
}

void CommandParserTest::testDrawCursor()
{
    Command c = creator_->moveCursor(1, 2);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(1, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(2, (int)writer_->getY());
    CPPUNIT_ASSERT(writer_->isCursorDisplayed());

}

void CommandParserTest::testEraseCursor()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testClearScreen()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testMoveCursorUp()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testMoveCursorDown()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testMoveCursorRight()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testMoveCursorLeft()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testMoveCursorTo()
{
    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testMoveCursor()
{
    CPPUNIT_ASSERT(false);
}

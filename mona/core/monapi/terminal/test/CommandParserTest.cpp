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
//    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testEraseCursor()
{
//    CPPUNIT_ASSERT(false);
}

void CommandParserTest::testClearScreen()
{
    Command c = creator_->clearScreen();
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT(writer_->isCleared());
}

void CommandParserTest::testMoveCursorUp()
{
    Command c = creator_->moveCursor(2, 2);
    parser_->parse(c.getBuffer(), c.getSize());
    c = creator_->moveCursorUp(1);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(2, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(1, (int)writer_->getY());
}

void CommandParserTest::testMoveCursorDown()
{
    Command c = creator_->moveCursor(2, 2);
    parser_->parse(c.getBuffer(), c.getSize());
    c = creator_->moveCursorDown(1);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(2, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(3, (int)writer_->getY());
}

void CommandParserTest::testMoveCursorRight()
{
    Command c = creator_->moveCursor(2, 2);
    parser_->parse(c.getBuffer(), c.getSize());
    c = creator_->moveCursorRight(10);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(12, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(2, (int)writer_->getY());
}

void CommandParserTest::testMoveCursorLeft()
{
    Command c = creator_->moveCursor(2, 2);
    parser_->parse(c.getBuffer(), c.getSize());
    c = creator_->moveCursorLeft(2);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(0, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(2, (int)writer_->getY());
}

void CommandParserTest::testMoveCursor()
{
    Command c = creator_->moveCursor(1, 2);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(1, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(2, (int)writer_->getY());

    c = creator_->moveCursor(11, 25);
    parser_->parse(c.getBuffer(), c.getSize());
    CPPUNIT_ASSERT_EQUAL(11, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(25, (int)writer_->getY());
}

void CommandParserTest::testMix()
{
    char buf[64];
    const char hello[] = "Hello\n";
    memcpy(buf, hello, strlen(hello));
    Command c = creator_->moveCursor(1, 2);
    memcpy(&buf[strlen(hello)], c.getBuffer(), c.getSize());
    Command c2 = creator_->moveCursorDown(1);
    memcpy(&buf[strlen(hello) + c.getSize()], c2.getBuffer(), c2.getSize());
    parser_->parse((uint8_t*)buf, strlen(hello) + c.getSize() + c2.getSize());
    CPPUNIT_ASSERT_EQUAL(1, (int)writer_->getX());
    CPPUNIT_ASSERT_EQUAL(3, (int)writer_->getY());
    CPPUNIT_ASSERT(writer_->getText() == "Hello\n");

}

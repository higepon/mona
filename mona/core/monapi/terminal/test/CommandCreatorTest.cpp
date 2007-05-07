#include "CommandCreatorTest.h"

using namespace MonAPI::terminal;

CPPUNIT_TEST_SUITE_REGISTRATION(CommandCreatorTest);

void CommandCreatorTest::setUp()
{
    creator_ = new CommandCreator();
}

void CommandCreatorTest::tearDown()
{
    delete creator_;
}

void CommandCreatorTest::testDrawCursor()
{
    const uint8_t COMMAND[] = {0x1b, '[', '7', 'm', ' ', 0x1b, '[', '2', '7', 'm'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    Command c = creator_->drawCursor();
    CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
    CPPUNIT_ASSERT(memcmp(COMMAND, c.getBuffer(), c.getSize()) == 0);
}

void CommandCreatorTest::testEraseCursor()
{
    const uint8_t COMMAND[]  = {0x1b, '[', '2', '7', 'm', ' ', 0x1b, '[', '7', 'm'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    Command c = creator_->eraseCursor();
    CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
    CPPUNIT_ASSERT(memcmp(COMMAND, c.getBuffer(), c.getSize()) == 0);
}

void CommandCreatorTest::testClearScreen()
{
    const uint8_t COMMAND[]  = {0x1b, '[', '2', 'J'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    Command c = creator_->clearScreen();
    CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
    CPPUNIT_ASSERT(memcmp(COMMAND, c.getBuffer(), c.getSize()) == 0);
}

void CommandCreatorTest::testMoveCursorUp()
{
    uint8_t command[] = {0x1b, '[', 'O', Command::CURSOR_UP};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t offset = 1;
        Command c = creator_->moveCursorUp(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 5;
        Command c = creator_->moveCursorUp(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }
}

void CommandCreatorTest::testMoveCursorDown()
{
    uint8_t command[]  = {0x1b, '[', 'O', Command::CURSOR_DOWN};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t offset = 1;
        Command c = creator_->moveCursorDown(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 5;
        Command c = creator_->moveCursorDown(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }
}

void CommandCreatorTest::testMoveCursorRight()
{
    uint8_t command[]  = {0x1b, '[', 'O', Command::CURSOR_RIGHT};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t offset = 1;
        Command c = creator_->moveCursorRight(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 5;
        Command c = creator_->moveCursorRight(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }
}

void CommandCreatorTest::testMoveCursorLeft()
{
    uint8_t command[]  = {0x1b, '[', 'O', Command::CURSOR_LEFT};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t offset = 1;
        Command c = creator_->moveCursorLeft(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 5;
        Command c = creator_->moveCursorLeft(offset);
        uint8_t expectedCommand[] = {0x1b, '[', '5', 'D'};
        CPPUNIT_ASSERT_EQUAL(sizeof(expectedCommand), c.getSize());
        CPPUNIT_ASSERT(memcmp(expectedCommand, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 10;
        Command c = creator_->moveCursorLeft(offset);
        uint8_t expectedCommand[] = {0x1b, '[', '1', '0', 'D'};
        CPPUNIT_ASSERT_EQUAL(sizeof(expectedCommand), c.getSize());
        CPPUNIT_ASSERT(memcmp(expectedCommand, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 128;
        Command c = creator_->moveCursorLeft(offset);
        uint8_t expectedCommand[] = {0x1b, '[', '1', '2', '8', 'D'};
        CPPUNIT_ASSERT_EQUAL(sizeof(expectedCommand), c.getSize());
        CPPUNIT_ASSERT(memcmp(expectedCommand, c.getBuffer(), c.getSize()) == 0);

   }
}

void CommandCreatorTest::testBackSpace()
{
    uint8_t command[]  = {0x08};
    const uint32_t COMMAND_SIZE = sizeof(command);
    Command c = creator_->backSpace();
    CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
    CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
}

void CommandCreatorTest::testMoveCursorTo()
{
    uint8_t command[]  = {0x1b, '[', 'O', 'D'};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t offset = 1;
        char direction = Command::CURSOR_UP;
        Command c = creator_->moveCursorTo(offset, direction);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        command[3] = direction;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 5;
        char direction = Command::CURSOR_LEFT;
        Command c = creator_->moveCursorTo(offset, direction);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        command[3] = direction;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }
}

void CommandCreatorTest::testMoveCursor()
{
    static uint8_t command[]  = {0x1b, '[', 'X', ';', 'Y', 'H'};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t x = 1;
        uint32_t y = 1;
        Command c = creator_->moveCursor(x, y);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + x;
        command[4] = '0' + y;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t x = 5;
        uint32_t y = 4;
        Command c = creator_->moveCursor(x, y);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + x;
        command[4] = '0' + y;

        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t x = 50;
        uint32_t y = 120;
        Command c = creator_->moveCursor(x, y);
        uint8_t expectedCommand[] = {0x1b, '[', '5', '0', ';', '1', '2', '0', 'H'};
        CPPUNIT_ASSERT_EQUAL(sizeof(expectedCommand), c.getSize());
        CPPUNIT_ASSERT(memcmp(expectedCommand, c.getBuffer(), c.getSize()) == 0);
    }


}

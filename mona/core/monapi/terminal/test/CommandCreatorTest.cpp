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
    const uint8_t COMMAND[]  = {0x1b, 'c'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    Command c = creator_->clearScreen();
    CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
    CPPUNIT_ASSERT(memcmp(COMMAND, c.getBuffer(), c.getSize()) == 0);
}

void CommandCreatorTest::testMoveCursorUp()
{
    uint8_t command[]  = {0x1b, '[', 'O', Command::CURSOR_UP};
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

    {
        uint32_t offset = 12;
        uint8_t expected[8];
        Command c = creator_->moveCursorUp(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE * 2, c.getSize());
        command[2] = '0' + 9;
        memcpy(expected, command, COMMAND_SIZE);
        command[2] = '0' + 3;
        memcpy(&expected[4], command, COMMAND_SIZE);
        CPPUNIT_ASSERT(memcmp(expected, c.getBuffer(), c.getSize()) == 0);
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

    {
        uint32_t offset = 12;
        uint8_t expected[8];
        Command c = creator_->moveCursorDown(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE * 2, c.getSize());
        command[2] = '0' + 9;
        memcpy(expected, command, COMMAND_SIZE);
        command[2] = '0' + 3;
        memcpy(&expected[4], command, COMMAND_SIZE);
        CPPUNIT_ASSERT(memcmp(expected, c.getBuffer(), c.getSize()) == 0);
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

    {
        uint32_t offset = 12;
        uint8_t expected[8];
        Command c = creator_->moveCursorRight(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE * 2, c.getSize());
        command[2] = '0' + 9;
        memcpy(expected, command, COMMAND_SIZE);
        command[2] = '0' + 3;
        memcpy(&expected[4], command, COMMAND_SIZE);
        CPPUNIT_ASSERT(memcmp(expected, c.getBuffer(), c.getSize()) == 0);
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
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[2] = '0' + offset;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t offset = 12;
        uint8_t expected[8];
        Command c = creator_->moveCursorLeft(offset);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE * 2, c.getSize());
        command[2] = '0' + 9;
        memcpy(expected, command, COMMAND_SIZE);
        command[2] = '0' + 3;
        memcpy(&expected[4], command, COMMAND_SIZE);
        CPPUNIT_ASSERT(memcmp(expected, c.getBuffer(), c.getSize()) == 0);
    }
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

    {
        uint32_t offset = 12;
        uint8_t expected[8];
        char direction = Command::CURSOR_LEFT;
        Command c = creator_->moveCursorTo(offset, direction);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE * 2, c.getSize());
        command[2] = '0' + 9;
        command[3] = direction;
        memcpy(expected, command, COMMAND_SIZE);
        command[2] = '0' + 3;
        memcpy(&expected[4], command, COMMAND_SIZE);
        CPPUNIT_ASSERT(memcmp(expected, c.getBuffer(), c.getSize()) == 0);
    }
}

void CommandCreatorTest::testMoveCursor()
{
    uint8_t command[]  = {0x1b, '[', '0', 'X', '0', 'Y', ';', 'H'};
    const uint32_t COMMAND_SIZE = sizeof(command);

    {
        uint32_t x = 0;
        uint32_t y = 0;
        Command c = creator_->moveCursor(x, y);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[3] = '0' + x;
        command[5] = '0' + y;
        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

    {
        uint32_t x = 5;
        uint32_t y = 4;
        Command c = creator_->moveCursor(x, y);
        CPPUNIT_ASSERT_EQUAL(COMMAND_SIZE, c.getSize());
        command[3] = '0' + x;
        command[5] = '0' + y;

        CPPUNIT_ASSERT(memcmp(command, c.getBuffer(), c.getSize()) == 0);
    }

}

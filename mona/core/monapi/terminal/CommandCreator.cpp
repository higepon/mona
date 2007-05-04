#include "monapi/terminal/CommandCreator.h"

using namespace MonAPI::terminal;

CommandCreator::CommandCreator()
{
}

CommandCreator::~CommandCreator()
{
}

Command CommandCreator::drawCursor()
{
    static const uint8_t COMMAND[] = {0x1b, '[', '7', 'm', ' ', 0x1b, '[', '2', '7', 'm'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    return Command(COMMAND, COMMAND_SIZE);
}

Command CommandCreator::eraseCursor()
{
    static const uint8_t COMMAND[] = {0x1b, '[', '2', '7', 'm', ' ', 0x1b, '[', '7', 'm'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    return Command(COMMAND, COMMAND_SIZE);
}

Command CommandCreator::clearScreen()
{
    static const uint8_t COMMAND[] = {0x1b, 'c'};
    const uint32_t COMMAND_SIZE = sizeof(COMMAND);
    return Command(COMMAND, COMMAND_SIZE);
}

Command CommandCreator::moveCursorTo(uint32_t n, char direction)
{
    const uint32_t BUFFER_SIZE = 512; // should be devided by COMMAND_SIZE.
    uint8_t command[]  = {0x1b, '[', 'O', 'D'};
    const uint32_t COMMAND_SIZE = sizeof(command);

    ASSERT(0 < n && n <= (BUFFER_SIZE / COMMAND_SIZE) * OFFSET_MAX);
    ASSERT('A' <= direction && direction <= 'D');

    static uint8_t buffer[BUFFER_SIZE];
    uint32_t rest = n;
    uint32_t commandTotalSize = 0;
    while (rest > 0)
    {
        uint32_t offset;
        if (rest > OFFSET_MAX)
        {
            offset = OFFSET_MAX;
            rest -= OFFSET_MAX;
        }
        else
        {
            offset = rest;
            rest = 0;
        }
        command[2] = '0' + offset;
        command[3] = direction;
        memcpy(&buffer[commandTotalSize], command, COMMAND_SIZE);
        commandTotalSize += COMMAND_SIZE;
    }
    return Command(buffer, commandTotalSize);
}

Command CommandCreator::moveCursor(uint32_t x, uint32_t y)
{
    ASSERT(0 <= x && x <= 9);
    ASSERT(0 <= y && y <= 9);
    static uint8_t command[]  = {0x1b, '[', '0', 'X', '0', 'Y', ';', 'H'};
    const uint32_t COMMAND_SIZE = sizeof(command);
    command[3] = '0' + x;
    command[5] = '0' + y;
    return Command(command, COMMAND_SIZE);
}

Command CommandCreator::moveCursorUp(uint32_t n)
{
    return moveCursorTo(n, Command::CURSOR_UP);
}

Command CommandCreator::moveCursorDown(uint32_t n)
{
    return moveCursorTo(n, Command::CURSOR_DOWN);
}

Command CommandCreator::moveCursorLeft(uint32_t n)
{
    return moveCursorTo(n, Command::CURSOR_LEFT);
}

Command CommandCreator::moveCursorRight(uint32_t n)
{
    return moveCursorTo(n, Command::CURSOR_RIGHT);
}

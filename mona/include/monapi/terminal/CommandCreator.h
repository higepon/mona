#ifndef _MONAPI_TERMINAL_COMMAND_CREATOR_
#define _MONAPI_TERMINAL_COMMAND_CREATOR_

#include "sys/types.h"
#include "monapi/Assert.h"

#ifdef TERMINAL_TEST
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#else
#include <monapi.h>
#endif

namespace MonAPI {
namespace terminal {

class Command
{
public:
    Command(const uint8_t* buffer, uint32_t size) : buffer_(buffer), size_(size) {}
    ~Command() {}

    const uint8_t* getBuffer() const { return buffer_; }
    uint32_t getSize() { return size_; }

    enum
    {
        CURSOR_UP    = 'A',
        CURSOR_DOWN  = 'B',
        CURSOR_RIGHT = 'C',
        CURSOR_LEFT  = 'D',
    };

protected:
    const uint8_t* buffer_;
    uint32_t size_;
};

class CommandCreator
{
public:
    CommandCreator();
    virtual ~CommandCreator();

    Command drawCursor();
    Command eraseCursor();
    Command clearScreen();
    Command moveCursor(uint32_t x, uint32_t y);
    Command moveCursorUp(uint32_t n);
    Command moveCursorDown(uint32_t n);
    Command moveCursorLeft(uint32_t n);
    Command moveCursorRight(uint32_t n);
    Command moveCursorTo(uint32_t n, char direction);
    Command backSpace();

protected:
    enum
    {
        OFFSET_MAX = 9,
    };
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_COMMAND_CREATOR_

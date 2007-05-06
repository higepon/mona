#ifndef _MONAPI_TERMINAL_WRITER_
#define _MONAPI_TERMINAL_WRITER_

#include "sys/types.h"
#include "monapi/Assert.h"

namespace MonAPI {
namespace terminal {

// interface of Terminal Output
class Writer
{
public:
    Writer() {}
    virtual ~Writer() {}

    virtual int drawCursor()                             = 0;
    virtual int eraseCursor()                            = 0;
    virtual int clearScreen()                            = 0;
    virtual int moveCursor(uint32_t x, uint32_t y)       = 0;
    virtual int moveCursorUp(uint32_t n)                 = 0;
    virtual int moveCursorDown(uint32_t n)               = 0;
    virtual int moveCursorLeft(uint32_t n)               = 0;
    virtual int moveCursorRight(uint32_t n)              = 0;
    virtual int write(uint8_t* buf, uint32_t length)     = 0;
    virtual uint32_t getX() const                        = 0;
    virtual uint32_t getY() const                        = 0;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_WRITER_

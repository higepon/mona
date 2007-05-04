#ifndef _MONAPI_TERMINAL_TEST_WRITER_
#define _MONAPI_TERMINAL_TEST_WRITER_

#include "sys/types.h"
#include "monapi/Assert.h"
#include "monapi/terminal/Writer.h"

namespace MonAPI {
namespace terminal {

class TestWriter : public Writer
{
public:
    TestWriter();
    virtual ~TestWriter();

    virtual int drawCursor();
    virtual int eraseCursor();
    virtual int clearScreen();
    virtual int moveCursor(uint32_t x, uint32_t y);
    virtual int moveCursorUp(uint32_t n);
    virtual int moveCursorDown(uint32_t n);
    virtual int moveCursorLeft(uint32_t n);
    virtual int moveCursorRight(uint32_t n);
    virtual int moveCursorTo(uint32_t n, char direction);

    uint32_t getX() const { return x_; }
    uint32_t getY() const { return y_; }
    bool isCursorDisplayed() const { return isCursorDisplayed_; }
protected:
    uint32_t x_;
    uint32_t y_;
    bool isCursorDisplayed_;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_TEST_WRITER_

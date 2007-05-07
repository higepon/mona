#ifndef _MONAPI_TERMINAL_TEST_WRITER_
#define _MONAPI_TERMINAL_TEST_WRITER_

#include <sys/types.h>
#include <stdint.h>
#include "monapi/Assert.h"
#include "monapi/terminal/Writer.h"
#include <string>


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
    virtual int lineFeed();
    virtual int backSpace();
    virtual int write(uint8_t* buf, uint32_t length);
    virtual uint32_t getX() const { return x_; }
    virtual uint32_t getY() const { return y_; }
    virtual std::string getText(){ return text_; }

    virtual bool isCleared() const { return isCleared_; }
protected:
    uint32_t x_;
    uint32_t y_;
    bool isCleared_;
    std::string text_;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_TEST_WRITER_

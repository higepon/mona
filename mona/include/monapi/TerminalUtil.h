#ifndef _MONAPI_TERMINALUTIL_
#define _MONAPI_TERMINALUTIL_

#include <sys/types.h>
#include <monapi/Stream.h>

namespace MonAPI {

class TerminalUtil
{
public:
    TerminalUtil(Stream* out);
    virtual ~TerminalUtil();

    int writeToOutBuffer();
    int drawCursor();
    int eraseCursor();
    int clearScreen();
    int cursorMove(uint32_t x, uint32_t y);
    int cursorUp(uint32_t n);
    int cursorDown(uint32_t n);
    int cursorLeft(uint32_t n);
    int cursorRight(uint32_t n);
    int cursorTo(uint32_t n, char direction);
    int write(const char* text);

protected:
    enum
    {
        CURSOR_UP    = 'A',
        CURSOR_DOWN  = 'B',
        CURSOR_RIGHT = 'C',
        CURSOR_LEFT  = 'D',
        OUT_BUFFER_SIZE = 512,
    };
    Stream* out_;
    char outbuffer_[OUT_BUFFER_SIZE];
};

}; // namespace MonAPI

#endif // _MONAPI_TERMINALUTIL_

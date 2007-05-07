#ifndef _MONAPI_TERMINAL_UTIL_
#define _MONAPI_TERMINAL_UTIL_

#include <sys/types.h>
#include <monapi/Stream.h>
#include <monapi/terminal/CommandCreator.h>

namespace MonAPI {
namespace terminal {

class Util
{
public:
    Util(Stream* out);
    virtual ~Util();

    int writeToOutBuffer(Command c);
    int drawCursor();
    int eraseCursor();
    int clearScreen();
    int backSpace();
    int moveCursor(uint32_t x, uint32_t y);
    int moveCursorUp(uint32_t n);
    int moveCursorDown(uint32_t n);
    int moveCursorLeft(uint32_t n);
    int moveCursorRight(uint32_t n);
    int moveCursorTo(uint32_t n, char direction);
    int write(const char* text);

protected:
    Stream* out_;
    CommandCreator creator_;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_UTIL_

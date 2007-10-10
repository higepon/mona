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
    int flush();
    int clearScreen();
    int backSpace();
    int moveCursor(uint32_t x, uint32_t y);
    int moveCursorUp(uint32_t n);
    int moveCursorDown(uint32_t n);
    int moveCursorLeft(uint32_t n);
    int moveCursorRight(uint32_t n);
    int moveCursorTo(uint32_t n, char direction);
    int write(const uint8_t* text, uint32_t size);
    int write(const char* text);
    uint32_t getCurrentCapacity() { return bufferSize_ - writtenSize_; }

protected:
    Stream* out_;
    uint32_t writtenSize_;
    uint32_t bufferSize_;
    uint8_t* buffer_;
    CommandCreator creator_;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_UTIL_

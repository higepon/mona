#include <monapi.h>
#include <monalibc.h>

using namespace MonAPI;

TerminalUtil::TerminalUtil(Stream* out) : out_(out)
{
}

TerminalUtil::~TerminalUtil()
{
}

int TerminalUtil::writeToOutBuffer()
{
   uint32_t expectedSize = strlen(outbuffer_);
   uint32_t wroteSize = out_->write((uint8_t*)outbuffer_, expectedSize);
   return expectedSize == wroteSize ? MONA_SUCCESS : MONA_FAILURE;
}

int TerminalUtil::write(const char* text)
{
    strcpy(outbuffer_, text);
    return writeToOutBuffer();
}

int TerminalUtil::drawCursor()
{
    outbuffer_[0] = 0x1b;
    memcpy(&outbuffer_[1], "[7m ", 4);
    outbuffer_[5] = 0x1b;
    memcpy(&outbuffer_[6], "[27m\0", 5);
    return writeToOutBuffer();
}

int TerminalUtil::eraseCursor()
{
    outbuffer_[0] = 0x1b;
    memcpy(&outbuffer_[1], "[27m ", 5);
    outbuffer_[6] = 0x1b;
    memcpy(&outbuffer_[7], "[7m\0", 3);
    return writeToOutBuffer();
}

int TerminalUtil::clearScreen()
{
    outbuffer_[0] = 0x1b;
    outbuffer_[1] = 'c';
    outbuffer_[2] = '\0';
    return writeToOutBuffer();
}

int TerminalUtil::cursorUp(uint32_t n)
{
    return cursorTo(n, CURSOR_UP);
}

int TerminalUtil::cursorDown(uint32_t n)
{
    return cursorTo(n, CURSOR_DOWN);
}

int TerminalUtil::cursorLeft(uint32_t n)
{
    return cursorTo(n, CURSOR_LEFT);
}

int TerminalUtil::cursorRight(uint32_t n)
{
    return cursorTo(n, CURSOR_RIGHT);
}

int TerminalUtil::cursorTo(uint32_t n, char direction)
{
    if (n <= 0) return 0;
    uint32_t rest = n;
    int ret;
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
        outbuffer_[0] = 0x1b;
        outbuffer_[1] = '[';
        outbuffer_[2] = '0' + offset;
        outbuffer_[3] = direction;
        outbuffer_[4] = '\0';
        ret = writeToOutBuffer();
    }
    return ret;
}

int TerminalUtil::cursorMove(uint32_t x, uint32_t y)
{
    char buffer[32];
    buffer[0] = 0x1b;
    buffer[1] = '[';
    buffer[2] = '0' + x;
    buffer[3] = '0' + y;
    buffer[4] = ';';
    buffer[5] = 'H';
    buffer[6] = '\0';
    uint32_t expectedSize = strlen(buffer);
    uint32_t wroteSize = out_->write((uint8_t*)buffer, strlen(buffer));
    return expectedSize == wroteSize ? MONA_SUCCESS : MONA_FAILURE;
}

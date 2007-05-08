#include <monapi.h>
#include <monalibc.h>
#include <monapi/terminal/Util.h>

using namespace MonAPI;
using namespace MonAPI::terminal;

Util::Util(Stream* out) : out_(out), writtenSize_(0)
{
}

Util::~Util()
{
}

int Util::writeToOutBuffer(Command c)
{
    if (writtenSize_ + c.getSize() > BUFFER_SIZE)
    {
        flush();
    }
    memcpy(&buffer_[writtenSize_], c.getBuffer(), c.getSize());
    writtenSize_ += c.getSize();
    return 0;
//     uint32_t wroteSize = out_->write((uint8_t*)c.getBuffer(), c.getSize());
//     return c.getSize() == wroteSize ? MONA_SUCCESS : MONA_FAILURE;
}

int Util::write(const char* text)
{
    return writeToOutBuffer(Command((const uint8_t*)text, strlen(text)));
}

int Util::backSpace()
{
    return writeToOutBuffer(creator_.backSpace());
}

int Util::drawCursor()
{
    return writeToOutBuffer(creator_.drawCursor());
}

int Util::eraseCursor()
{
    return writeToOutBuffer(creator_.eraseCursor());
}

int Util::clearScreen()
{
    return writeToOutBuffer(creator_.clearScreen());
}

int Util::moveCursorUp(uint32_t n)
{
    return writeToOutBuffer(creator_.moveCursorUp(n));
}

int Util::moveCursorDown(uint32_t n)
{
    return writeToOutBuffer(creator_.moveCursorDown(n));
}

int Util::moveCursorLeft(uint32_t n)
{
    return writeToOutBuffer(creator_.moveCursorLeft(n));
}

int Util::moveCursorRight(uint32_t n)
{
    return writeToOutBuffer(creator_.moveCursorRight(n));
}

int Util::moveCursorTo(uint32_t n, char direction)
{
    return writeToOutBuffer(creator_.moveCursorTo(n, direction));
}

int Util::moveCursor(uint32_t x, uint32_t y)
{
    return writeToOutBuffer(creator_.moveCursor(x, y));
}

int Util::flush()
{
    if (writtenSize_ == 0) return 0;
    uint32_t wroteSize = out_->write(buffer_, writtenSize_);
    bool ok = writtenSize_ == wroteSize;
    writtenSize_ = 0;
    return ok ? MONA_SUCCESS : MONA_FAILURE;
}

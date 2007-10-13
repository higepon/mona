#include <monapi.h>
#include <monalibc.h>
#include <monapi/terminal/Util.h>
#include <monalibc/assert.h>

using namespace MonAPI;
using namespace MonAPI::terminal;

Util::Util(Stream* out) : out_(out), writtenSize_(0)
{
    bufferSize_ = out_->capacity();
    buffer_ = new uint8_t[bufferSize_];
}

Util::~Util()
{
    delete[] buffer_;
}

int Util::writeToOutBuffer(Command c)
{
    return write(c.getBuffer(), c.getSize());
}

int Util::write(const char* text)
{
    write((const uint8_t*)text, strlen(text) + 1);
    return 0;
}

int Util::write(const uint8_t* text, uint32_t size)
{
    while (size > 0)
    {
        uint32_t currentCapacity = getCurrentCapacity();
        if (0 == currentCapacity)
        {
            flush();
            currentCapacity = getCurrentCapacity();
        }
        uint32_t writeSize = size > getCurrentCapacity() ? getCurrentCapacity() : size;
        memcpy(&buffer_[writtenSize_], text, writeSize);
        writtenSize_ += writeSize;
        size -= writeSize;
    }
    return 0;
}

int Util::backSpace()
{
    return writeToOutBuffer(creator_.backSpace());
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
    uint8_t* p = buffer_;
    while (writtenSize_ > 0)
    {
        out_->waitForWrite();
        uint32_t wroteSize = out_->write(p, writtenSize_);
        p += wroteSize;
        writtenSize_ -= wroteSize;
    }
    return MONA_SUCCESS;
}

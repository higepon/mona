#include "TestWriter.h"

using namespace MonAPI::terminal;

TestWriter::TestWriter() : x_(0), y_(0), isCleared_(false)
{

}

TestWriter::~TestWriter()
{

}

int TestWriter::drawCursor()
{
    return 0;
}

int TestWriter::eraseCursor()
{
    return 0;
}

int TestWriter::write(uint8_t* buf, uint32_t length)
{
    text_ += (char*)buf;
    return 0;
}

int TestWriter::clearScreen()
{
    isCleared_ = true;
    return 0;
}

int TestWriter::moveCursor(uint32_t x, uint32_t y)
{
    x_ = x;
    y_ = y;
    return 0;
}

int TestWriter::moveCursorUp(uint32_t n)
{
    y_ -= n;
    if (y_ < 0) y_ = 0;
    return 0;
}

int TestWriter::moveCursorDown(uint32_t n)
{
    y_ += n;
    return 0;
}

int TestWriter::moveCursorLeft(uint32_t n)
{
    x_ -= n;
    if (x_ < 0) x_ = 0;
    return 0;
}

int TestWriter::moveCursorRight(uint32_t n)
{
    x_ += n;
    return 0;
}

int TestWriter::lineFeed()
{
    text_ += '\n';
    return 0;
}

#include "ScreenWriter.h"

using namespace MonAPI::terminal;

ScreenWriter::ScreenWriter() : x_(0), y_(0)
{

}

ScreenWriter::~ScreenWriter()
{

}

int ScreenWriter::drawCursor()
{
}

int ScreenWriter::eraseCursor()
{

}

int ScreenWriter::clearScreen()
{
}

int ScreenWriter::moveCursor(uint32_t x, uint32_t y)
{
    syscall_set_cursor(x, y);
    x_ = x;
    y_ = y;
}

int ScreenWriter::moveCursorUp(uint32_t n)
{
    y_ -= n;
    if (y_ < 0) y_ = 0;
    return 0;
}

int ScreenWriter::moveCursorDown(uint32_t n)
{
    y_ += n;
    return 0;
}

int ScreenWriter::moveCursorLeft(uint32_t n)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - n, y);
    return 0;
}

int ScreenWriter::moveCursorRight(uint32_t n)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x + n, y);
    return 0;
}

int ScreenWriter::write(uint8_t* buf, uint32_t length)
{
    syscall_print((char*)buf);
}

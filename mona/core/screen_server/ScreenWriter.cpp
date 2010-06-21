#include "ScreenWriter.h"

using namespace MonAPI;
using namespace MonAPI::terminal;

#define FONT_WIDTH   8
#define FONT_HEIGHT 16
#define FOREGROUND 0x000000
#define BACKGROUND 0xffffff

extern Screen screen;

static void cursor(bool erase)
{
    int x, y;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    syscall_get_cursor(&x, &y);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    screen.fillRect16(x * FONT_WIDTH, y * FONT_HEIGHT + FONT_HEIGHT - 2, 8, 2,
                      erase ? BACKGROUND : FOREGROUND);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}


ScreenWriter::ScreenWriter() : x_(0), y_(0)
{

}

ScreenWriter::~ScreenWriter()
{

}

int ScreenWriter::clearScreen()
{
    return 0; // ToDo
}

int ScreenWriter::moveCursor(uint32_t x, uint32_t y)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    cursor(true);
    syscall_set_cursor(x, y);
    x_ = x;
    y_ = y;
    cursor(false);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

int ScreenWriter::moveCursorUp(uint32_t n)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    y_ -= n;
    if (y_ < 0) y_ = 0;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

int ScreenWriter::moveCursorDown(uint32_t n)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    y_ += n;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

int ScreenWriter::moveCursorLeft(uint32_t n)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    int x, y;
    cursor(true);
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - n, y);
    syscall_get_cursor(&x, &y);
    cursor(false);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

int ScreenWriter::moveCursorRight(uint32_t n)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    int x, y;
    cursor(true);
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x + n, y);
    cursor(false);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

int ScreenWriter::write(uint8_t* buf, uint32_t length)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    syscall_print((char*)buf);
    cursor(false);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

int ScreenWriter::lineFeed()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
   cursor(true);
   syscall_print("\n");
   cursor(false);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
   return 0;
}

int ScreenWriter::backSpace()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
   cursor(true);
   moveCursorLeft(1);
   cursor(false);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
   return 0;
}

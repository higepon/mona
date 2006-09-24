/*!
    \file  VesaConsole.cpp
    \brief class VesaConsole

    class VesaConsole

    Copyright (c) 2004 Gaku
    All rights reserved.
    License=MIT/X License

    \author  Gaku
    \version $Revision$
    \date   create:2004/01/22 update:$Date$
*/

#include "VesaConsole.h"

extern "C" dword get_font_address();

dword VesaConsole::palette[] = {
                        0x00000000, // black
                        0x000000ff, // blue
                        0x0000ff00, // green
                        0x0000ffff, // skyblue
                        0x00ff0000, // red
                        0x00ff00ff, // pink
                        0x00808000, // orange
                        0x00c0c0c0, // gray
                        0x00808080, // darkgray
                        0x00000080, // lightblue
                        //                        0x00008080, // lightbue
                        0x00D9C4D1, // lightbue
                        //                        0x00008000, // lightgreen
                        0x0096A48E, // lightgreen
                        0x00800000, // lightred
                        0x00800080, // darkpink
                        0x00ffff00, // yellow
                        0x00ffffff  // white
                };

VesaConsole::VesaConsole (VesaInfoDetail *info)
    : screen(info)
{
    xResolution_ = info->xResolution;
    yResolution_ = info->yResolution;
    pos_x_ = 0;
    pos_y_ = 0;

    byte* font = (byte*)get_font_address();
    font_x_ = font[14];
    font_y_ = font[15];
    font_ = font + 17;
    console_x_ = xResolution_ / font_x_;
    console_y_ = yResolution_ / font_y_;

    int len = console_x_ * console_y_;
    char_buffer_ = new char[len];
    palette_buffer_ = new dword[len];

    bg_ = getColor(GP_BLACK);
    ch_ = getColor(GP_WHITE);

    clearScreen();
}

void VesaConsole::printf(const char *format, ...)
{
    void** list = (void **)&format;

    list++;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  print((char *)*list);
                  list++;
                  break;
              case 'd':
                  putInt(*(int*)list, 10);
                  list++;
                  break;
              case 'x':
                  print("0x");
                  putInt(*(int*)list, 16);
                  list++;
                  break;
              case 'c':
                  putCharacter((char)*(int*)(list));
                  list++;
                  break;
              case '%':
                  putCharacter('%');
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            putCharacter(format[i]);
        }
    }
}

void VesaConsole::putInt(size_t n, int base) {

    static char buf[256];
    int geta;
    int num = n;

    if (base != 16) {

        for (geta = 0; num; num /= 10, geta++);
        if ((int)n < 0) {
            geta++;
            base *= -1;
        }
    } else {
        geta = 8;
    }

    char* p = ltona(n, buf, geta, base);
    print(p);
}

void VesaConsole::setBGColor(const char color)
{
    bg_ = getColor(color);
}

void VesaConsole::setCHColor(const char color)
{
    ch_ = getColor(color);
}

void VesaConsole::putCharacter(char ch)
{

    ch = (unsigned char)ch;

    if ('\n' == ch) {
        newLine();
        return;
    }

    int idx = pos_x_ + pos_y_ * console_x_;
    char_buffer_[idx] = ch;
    palette_buffer_[idx] = ch_;

    if (ch == '\xff') ch = ' ';
    byte* p = font_ + 16 * ch;
    screen.fillPat(pos_x_ * font_x_, pos_y_ * font_y_, font_x_, font_y_, ch_, bg_, p);

    nextCursor();
}

void VesaConsole::print(char* str)
{
    for (int i = 0; '\0' != str[i]; i++) {
        putCharacter(str[i]);
    }
}

void VesaConsole::locate(int x, int y)
{
    if (0 > x) x = 0;
    if (0 > y) y = 0;
    if (console_x_ <= x) x = console_x_ - 1;
    if (console_y_ <= y) y = console_y_ - 1;

    pos_x_ = x;
    pos_y_ = y;
}

void VesaConsole::setCursor(int x, int y)
{
    locate(x, y);
}

void VesaConsole::getCursor(int* x, int* y)
{
    *x = pos_x_;
    *y = pos_y_;
}

void VesaConsole::clearScreen()
{
    int len = console_x_ * console_y_;
     for (int i = 0; i < len; i++) char_buffer_[i] = '\0';
    screen.clearScreenWhite(xResolution_, yResolution_);
}

dword VesaConsole::getColor (char c)
{
    if (GP_BLACK > c) c = GP_BLACK;
    if (GP_WHITE < c) c = GP_WHITE;

    return palette[(int)c];
}

void VesaConsole::nextCursor ()
{
    pos_x_++;

    if (pos_x_ >= console_x_) newLine();
}

void VesaConsole::newLine ()
{
    pos_x_ = 0;
    pos_y_++;
    if (pos_y_ < console_y_) return;

    pos_y_ = console_y_ - 1;
#if 1  // Tino: speed up
    int len = console_x_ * console_y_;
    for (int i = console_x_, j = 0, x = 0, y = 0; i < len; i++, j++)
    {
        char c_i = char_buffer_[i], c_j = char_buffer_[j];
        dword p_i = palette_buffer_[i], p_j = palette_buffer_[j];
        if (c_i == '\xff')
        {
            screen.bitblt(x * font_x_, y * font_y_, x * font_x_, (y + 1) * font_y_, font_x_, font_y_);
        }
        else if (c_i != '\0')
        {
            if (c_i != c_j || p_i != p_j)
            {
                byte* p = font_ + 16 * c_i;
                screen.fillPat(x * font_x_, y * font_y_, font_x_, font_y_, p_i, bg_, p);
            }
        }
        else if (c_j != '\0')
        {
            screen.fill(x * font_x_, y * font_y_, font_x_, font_y_, bg_);
        }
        char_buffer_[j] = c_i;
        palette_buffer_[j] = p_i;
        x++;
        if (x >= console_x_)
        {
            x = 0;
            y++;
        }
    }
    for (int i = 0, j = pos_y_ * console_x_; i < console_x_; i++, j++)
    {
        char_buffer_[j] = '\0';
    }
#else
    screen.scrollUp(font_y_, pos_y_ * font_y_);
#endif
    screen.fill(0, pos_y_ * font_y_, xResolution_, font_y_, bg_);
}

VesaConsole::VesaScreen::VesaScreen (VesaInfoDetail *info)
{
    vramAddress = info->physBasePtr;
    bytesPerScanLine = info->bytesPerScanLine;
    bitsPerPixel = info->bitsPerPixel;

    selectMethod(info);
}

void VesaConsole::VesaScreen::scrollUp (int y, int h)
{
    byte *dst = (byte*)vramAddress;
    byte *src = (byte*)vramAddress + bytesPerScanLine * y;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < bytesPerScanLine; j++) {
            if (dst[j] != src[j]) dst[j] = src[j];
        }
        dst += bytesPerScanLine;
        src += bytesPerScanLine;
    }
}

void VesaConsole::VesaScreen::bitblt (int dst_x, int dst_y, int src_x, int src_y, int w, int h)
{
    dword bytesPerPixel = bitsPerPixel/8, wb = w * bytesPerPixel;
    byte* dst = (byte*)vramAddress + bytesPerScanLine * dst_y + bytesPerPixel * dst_x;
    byte* src = (byte*)vramAddress + bytesPerScanLine * src_y + bytesPerPixel * src_x;
    for (int i = 0; i < h; i++)
    {
        memcpy(dst, src, wb);
        dst += bytesPerScanLine;
        src += bytesPerScanLine;
    }
}

void VesaConsole::VesaScreen::fill (int x, int y, int w, int h, dword c)
{
    dword bytesPerPixel = bitsPerPixel/8;
    byte *bits = (byte*)vramAddress
        + bytesPerScanLine * y + bytesPerPixel * x;
    byte *temp = bits;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            (this->*packColor)(temp, c);
            temp += bytesPerPixel;
        }
        bits += bytesPerScanLine;
        temp = bits;
    }
}

void *memsetWord(void* buf, word value, size_t times) {

    word *p = (word*)buf;

    while (times > 0) {
        *p = value;
        p++;
        times--;
    }
    return buf;
}

void VesaConsole::VesaScreen::clearScreenWhite(int w, int h)
{
#if 0
    const int buffer_size = 512;
    int size = w * h * (bitsPerPixel / 8) / buffer_size;
    byte* buffer[buffer_size];
    memset(buffer, 0xFF, buffer_size);
    for (int i = 0; i < size; i++)
    {
        memcpy((void*)(vramAddress + i * buffer_size), buffer, buffer_size);
    }
#else
    // enough speed at -O3
    memset((void*)vramAddress, 0xff, w * h * (bitsPerPixel / 8));
#endif
}

void VesaConsole::VesaScreen::clearScreenBlack(int w, int h)
{
    memset((void*)vramAddress, 0x00, w * h * (bitsPerPixel / 8));
}

void VesaConsole::VesaScreen::fillPat
    (int x, int y, int w, int h, dword c, dword b, byte* p)
{
#if 0
    dword bgcolor = (this->*getColor)(b);
    dword color = (this->*getColor)(c);

    dword bytesPerPixel = bitsPerPixel/8;
    static bool first = true;
    word* pixel;
    p = (byte*)get_font_address() + 17 + 16 * '3';
    if (first)
    {
        int index = 0;
        int k = 0x80;

        pixel = new word[16 * 16];
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                pixel[i * 16 + j] = p[index] & k ? color : bgcolor;

                k >>= 1;
                if (0 == k) {
                    k = 0x80;
                    index++;
                }
            }
            if (0x80 != k) {
                k = 0x80;
                index++;
            }
        }
        first = false;
    }

    word *bits = (word*)vramAddress
        + bytesPerScanLine / 2 * y + bytesPerPixel /2 * x;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            bits[i * w + j] = pixel[i * w + j];
        }
        bits += bytesPerScanLine / 2;
    }

#else
    dword bytesPerPixel = bitsPerPixel/8;
    byte *bits = (byte*)vramAddress
            + bytesPerScanLine * y + bytesPerPixel * x;
    byte *temp = bits;
    dword bgcolor = (this->*getColor)(b);
    dword color = (this->*getColor)(c);

    int index = 0;
    int k = 0x80;

    for (int i = 0; i < h; i++) {

        for (int j = 0; j < w; j++) {
            if (k & p[index]) {
                (this->*setColor)(temp, color);
            } else {
                (this->*setColor)(temp, bgcolor);
            }
            temp += bytesPerPixel;

            k >>= 1;
            if (0 == k) {
                k = 0x80;
                index++;
            }
        }
        bits += bytesPerScanLine;
        temp = bits;

        if (0x80 != k) {
            k = 0x80;
            index++;
        }
    }
#endif
}

void VesaConsole::VesaScreen::selectMethod (VesaInfoDetail *info)
{
    if (8 == info->redMaskSize && 8 == info->greenMaskSize && 8 == info->blueMaskSize)
    {
        packColor = &VesaConsole::VesaScreen::packColor24;
        getColor = &VesaConsole::VesaScreen::getColor24;
        setColor = &VesaConsole::VesaScreen::setColor24;
    }
    else
    {
        if (5 == info->redMaskSize && 6 == info->greenMaskSize && 5 == info->blueMaskSize)
        {
            packColor = &VesaConsole::VesaScreen::packColor16;
            getColor = &VesaConsole::VesaScreen::getColor16;
            setColor = &VesaConsole::VesaScreen::setColor16;
        }
        else
        {
            if (5 == info->redMaskSize && 5 == info->greenMaskSize && 5 == info->blueMaskSize)
            {
                packColor = &VesaConsole::VesaScreen::packColor15;
                getColor = &VesaConsole::VesaScreen::getColor15;
                setColor = &VesaConsole::VesaScreen::setColor24;
            }
            else
            {
                packColor = &VesaConsole::VesaScreen::packColor8;
                getColor = &VesaConsole::VesaScreen::getColor8;
                setColor = &VesaConsole::VesaScreen::setColor8;
            }
        }
    }
}

void VesaConsole::VesaScreen::packColor8 (byte *bits, dword c)
{
    *bits = ((c>>16) & 0xe0) | ((c>>8) & 0x18) | (c & 0x3);
}

void VesaConsole::VesaScreen::packColor15 (byte *bits, dword c)
{
    *((word*)bits) = ((c>>9) & 0x7c00)
                   | ((c>>6) & 0x03e0)
                   | ((c>>3) & 0x001f);
}

void VesaConsole::VesaScreen::packColor16 (byte *bits, dword c)
{
    *((word*)bits) = ((c>>8) & 0xf800)
                   | ((c>>5) & 0x07e0)
                   | ((c>>3) & 0x001f);
}

void VesaConsole::VesaScreen::packColor24 (byte *bits, dword c)
{
    //    *((dword*)bits) = c;
    bits[0] = (  c      & 0xff );
    bits[1] = ( (c>> 8) & 0xff );
    bits[2] = ( (c>>16) & 0xff );
}

dword VesaConsole::VesaScreen::getColor8 (dword c)
{
    return ((c>>16) & 0xe0) | ((c>>8) & 0x18) | (c & 0x3);
}

dword VesaConsole::VesaScreen::getColor15 (dword c)
{
    return ((c>>9) & 0x7c00)
        | ((c>>6) & 0x03e0)
        | ((c>>3) & 0x001f);
}

dword VesaConsole::VesaScreen::getColor16 (dword c)
{
    return ((c>>8) & 0xf800)
        | ((c>>5) & 0x07e0)
        | ((c>>3) & 0x001f);
}


dword VesaConsole::VesaScreen::getColor24 (dword c)
{
    return c;
}

void VesaConsole::VesaScreen::setColor8 (void* p, dword c)
{
    if (*((byte*)p) == c) return;
    *((byte*)p) = c;
}

void VesaConsole::VesaScreen::setColor15 (void* p, dword c)
{
    if (*((word*)p) == c) return;
    *((word*)p) = c;
}

void VesaConsole::VesaScreen::setColor16 (void* p, dword c)
{
    if (*((word*)p) == c) return;
    *((word*)p) = c;
}

void VesaConsole::VesaScreen::setColor24 (void* p, dword c)
{
    byte* bits = (byte*)p;
    bits[0] = (  c      & 0xff );
    bits[1] = ( (c>> 8) & 0xff );
    bits[2] = ( (c>>16) & 0xff );
}

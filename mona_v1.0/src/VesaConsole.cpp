/*!
    \file  VesaConsole.cpp
    \brief class VesaConsole

    class VesaConsole

    Copyright (c) 2004 Gaku
    All rights reserved.
    License=MIT/X Licnese

    \author  Gaku
    \version $Revision$
    \date   create:2004/01/22 update:$Date$
*/

#include <VesaConsole.h>

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
                        0x00008080, // lightbue
                        0x00008000, // lightgreen
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

        bg_ = getColor(GP_BLACK);
        ch_ = getColor(GP_WHITE);
        clearScreen();
}

void VesaConsole::printf(const char *format, ...)
{
    void** list = (void **)&format;

    ((char**)list) += 1;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            i++;

            switch (format[i]) {
              case 's':
                  print((char *)*list);
                  ((char**)list) += 1;
                  break;
              case 'd':
                  printInt((int)*list);
                  ((int*)list) += 1;
                  break;
              case 'x':
                  print("0x");
                  putInt((int)*list, 16);
                  ((int*)list) += 1;
                  break;
              case 'c':
                  putCharacter((char)(int)(*list));
                  ((char*)list) += 1;
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

        byte* p = font_ + 16 * ch;
        screen.fillPat(pos_x_, pos_y_, font_x_, font_y_, ch_, bg_, p);
        nextCursor();
}

void VesaConsole::print(char* str)
{
        for (int i = 0; '\0' != str[i]; i++) {
                putCharacter(str[i]);
        }
}

void VesaConsole::printInt(int num)
{
    char revstr[20];
    char str[20];
    int  i = 0;
    int  j = 0;

    /* negative number */
    if (num < 0) {
        str[0] = '-';
        j++;
        num = num * -1;
    }

    /* number to buffer */
    do {
        revstr[i] = '0' + (int)(num % 10);
        num = num / 10;
        i++;
    } while (num != 0);
    revstr[i] = '\0';

    /* revert */
    for (; i >= 0; j++) {
        str[j] = revstr[i - 1];
        i--;
    }

    /* print */
    print(str);
}

void VesaConsole::putInt(size_t n, int base)
{
    int    power;
    size_t num = n;
    size_t ch;

    for (power = 0; num != 0; power++) {
        num /= base;
    }

    for (int j = 0; j < 8 - power; j++) {
        putCharacter('0');
    }

    for (int i = power -1; i >= 0; i--) {
        ch = n / _power(base, i);
        n %= _power(base, i);

        if (i == 0 && n > 9) {

            putCharacter('A' + n -10);
        } else if (i == 0) {

            putCharacter('0' + n);
        } else if (ch > 9) {

            putCharacter('A' + ch -10);
        } else {

            putCharacter('0' + ch);
        }
    }
}

void VesaConsole::locate(int x, int y)
{
        if (0 > x) x = 0;
        if (0 > y) y = 0;
        if (console_x_ <= x) x = console_x_ - 1;
        if (console_y_ <= y) y = console_y_ - 1;

        pos_x_ = x * font_x_;
        pos_y_ = y * font_y_;
}

void VesaConsole::clearScreen()
{
        screen.fill(0, 0, xResolution_, yResolution_, getColor(bg_));
}

dword VesaConsole::getColor (char c)
{
        if (GP_BLACK > c) c = GP_BLACK;
        if (GP_WHITE < c) c = GP_WHITE;

        return palette[c];
}

void VesaConsole::nextCursor ()
{
        pos_x_ += font_x_;

        if (xResolution_ < pos_x_ + font_x_) {
                newLine();
        }
}

void VesaConsole::newLine ()
{
        pos_x_ = 0;
        pos_y_ += font_y_;

        if (yResolution_ < pos_y_ + font_y_) {
                pos_y_ -= font_y_;
                screen.scrollUp(font_y_, pos_y_);
                screen.fill(0, pos_y_, xResolution_, font_y_, bg_);
        }
}

size_t VesaConsole::_power(size_t x, size_t y) {

    size_t result = x;

    for (size_t i = 1; i < y; i++) {
        result *= x;
    }
    return result;
}

VesaConsole::VesaScreen::VesaScreen (VesaInfoDetail *info)
{
        vramAddress = info->physBasePtr;
        bytesPerScanLine = info->bytesPerScanLine;
        bitsPerPixel = info->bitsPerPixel;
}

word VesaConsole::VesaScreen::pack15bppColor (dword c)
{
        // 1-5-5-5
        word w = ((c>>9) & 0x7c00)
               | ((c>>6) & 0x03e0)
               | ((c>>3) & 0x001f);
        return w;
}

word VesaConsole::VesaScreen::pack16bppColor (dword c)
{
        // 5-6-5
        word w = ((c>>8) & 0xf800)
               | ((c>>5) & 0x07e0)
               | ((c>>3) & 0x001f);
        return w;
}

void VesaConsole::VesaScreen::scrollUp (int y, int h)
{
        byte *dst = (byte*)vramAddress;
        byte *src = (byte*)vramAddress + bytesPerScanLine * y;

        for (int i = 0; i < h; i++) {
                for (int j = 0; j < bytesPerScanLine; j++) {
                        dst[j] = src[j];
                }
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

        word color = pack16bppColor(c);

        for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                        *((word*)temp) = (word)color;
                        temp += bytesPerPixel;
                }
                bits += bytesPerScanLine;
                temp = bits;
        }
}

void VesaConsole::VesaScreen::fillPat
        (int x, int y, int w, int h, dword c, dword b, byte* p)
{
        dword bytesPerPixel = bitsPerPixel/8;
        byte *bits = (byte*)vramAddress
                + bytesPerScanLine * y + bytesPerPixel * x;
        byte *temp = bits;

        word ch = pack16bppColor(c);
        word bg = pack16bppColor(b);

        int index = 0;
        int k = 0x80;

        for (int i = 0; i < h; i++) {

                for (int j = 0; j < w; j++) {
                        if (k & p[index]) {
                                *((word*)temp) = ch;
                        } else {
                                *((word*)temp) = bg;
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
}

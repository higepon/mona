/*!
    \file VesaConsole.h
*/

#ifndef _VESA_CONSOLE_
#define _VESA_CONSOLE_

#include <sys/types.h>
#include "VirtualConsole.h"
#include "GraphicalConsole.h"
#include "vbe.h"

class VesaConsole : public VirtualConsole
{
public:
    VesaConsole (VesaInfoDetail *info);

    void printf(const char *, ...);
    void setBGColor(const char color);
    void setCHColor(const char color);
    void getCursor(int* x, int* y);
    void setCursor(int x, int y);
    void locate(int x, int y);
    void clearScreen();

private:
    static dword palette[];

    word xResolution_;
    word yResolution_;
    word pos_x_;
    word pos_y_;
    word font_x_;
    word font_y_;
    word console_x_;
    word console_y_;

    byte* font_;
    dword bg_;
    dword ch_;

    dword getColor (char color);
    void nextCursor ();
    void newLine ();
    void print(char* str);
    void putInt(size_t n, int base);
    void putCharacter(char ch);

    class VesaScreen
    {
    public:
        VesaScreen (VesaInfoDetail *info);

        void scrollUp (int y, int h);

        void fill (int x, int y, int w, int h, dword c);
        void fillPat (int x, int y, int w, int h, dword c, dword b, byte* p);

    private:
        void selectMethod (VesaInfoDetail *info);

        void packColor8 (byte *bits, dword c);
        void packColor15 (byte *bits, dword c);
        void packColor16 (byte *bits, dword c);
        void packColor24 (byte *bits, dword c);

        void (VesaConsole::VesaScreen::* packColor) (byte *bits, dword c);

        dword vramAddress;
        word bytesPerScanLine;
        word bitsPerPixel;
    };

    VesaScreen screen;
};

#endif // _VESA_CONSOLE_ //

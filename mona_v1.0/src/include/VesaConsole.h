/*!
    \file VesaConsole.h
*/

#ifndef _VESA_CONSOLE_
#define _VESA_CONSOLE_

#include <VirtualConsole.h>
#include <GraphicalConsole.h>
#include <vbe.h>
#include <types.h>

class VesaConsole : public VirtualConsole
{
public:
    VesaConsole (VesaInfoDetail *info);

    void printf(const char *, ...);
    void setBGColor(const char color);
    void setCHColor(const char color);
    void putCharacter(char ch);
    void print(char* str);
    void printInt(int num);
    void putInt(size_t n, int base);
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
    size_t _power(size_t x, size_t y);

    class VesaScreen
    {
    public:
        VesaScreen (VesaInfoDetail *info);

        word pack15bppColor (dword c);
        word pack16bppColor (dword c);

        void scrollUp (int y, int h);
        void fill (int x, int y, int w, int h, dword c);
        void fillPat (int x, int y, int w, int h, dword c, dword b, byte* p);

    private:
        dword vramAddress;
        word bytesPerScanLine;
        word bitsPerPixel;
    };

    VesaScreen screen;
};

#endif // _VESA_CONSOLE_ //

/*!
    \file VesaConsole.h
*/

#ifndef __VESACONSOLE_H__
#define __VESACONSOLE_H__

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

    char* char_buffer_;
    dword* palette_buffer_;

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

        byte* vramAddress;
        word bytesPerScanLine;
        word bitsPerPixel;
    };

    VesaScreen screen;
};

#endif

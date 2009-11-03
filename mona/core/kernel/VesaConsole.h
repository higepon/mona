/*!
    \file VesaConsole.h
*/

#ifndef _VESA_CONSOLE_
#define _VESA_CONSOLE_

#include <sys/types.h>
#include "VirtualConsole.h"
#include "GraphicalConsole.h"
#include "vbe.h"
#include "vsprintf.h"

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
    static uint32_t palette[];

    uint16_t xResolution_;
    uint16_t yResolution_;
    uint16_t pos_x_;
    uint16_t pos_y_;
    uint16_t font_x_;
    uint16_t font_y_;
    uint16_t console_x_;
    uint16_t console_y_;

    uint8_t* font_;
    uint32_t bg_;
    uint32_t ch_;

    char* char_buffer_;
    uint32_t* palette_buffer_;

    uint32_t getColor (char color);
    void nextCursor ();
    void newLine ();
    void print(const char* str);
//    void putInt(size_t n, int base);
    void putCharacter(char ch);

    class VesaScreen
    {
    public:
        VesaScreen (VesaInfoDetail *info);

        void scrollUp (int y, int h);
        void bitblt (int dst_x, int dst_y, int src_x, int src_y, int w, int h);

        void fill (int x, int y, int w, int h, uint32_t c);
        void clearScreenWhite(int w, int h);
        void clearScreenBlack(int w, int h);
        void clear (int w, int h, uint32_t c);
        void fillPat (int x, int y, int w, int h, uint32_t c, uint32_t b, uint8_t* p);

    private:
        void selectMethod (VesaInfoDetail *info);

        void packColor8 (uint8_t *bits, uint32_t c);
        void packColor15 (uint8_t *bits, uint32_t c);
        void packColor16 (uint8_t *bits, uint32_t c);
        void packColor24 (uint8_t *bits, uint32_t c);

        uint32_t getColor8 (uint32_t c);
        uint32_t getColor15 (uint32_t c);
        uint32_t getColor16 (uint32_t c);
        uint32_t getColor24 (uint32_t c);
        void setColor8 (void* p, uint32_t c);
        void setColor15 (void* p, uint32_t c);
        void setColor16 (void* p, uint32_t c);
        void setColor24 (void* p, uint32_t c);

        void (VesaConsole::VesaScreen::* packColor) (uint8_t *bits, uint32_t c);
        uint32_t (VesaConsole::VesaScreen::* getColor) (uint32_t c);
        void (VesaConsole::VesaScreen::* setColor) (void* p, uint32_t c);

        uint32_t vramAddress;
        uint16_t uint8_tsPerScanLine;
        uint16_t bitsPerPixel;
        uint16_t bytesPerPixel;
    };

    VesaScreen screen;
};

#endif // _VESA_CONSOLE_ //

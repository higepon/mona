/*----------------------------------------------------------------------
    Screen
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Screen::Screen() {

    volatile ScreenInfo sinfo;

    /* get and set vram information */
    syscall_get_vram_info(&sinfo);
    vram_        = (byte*)sinfo.vram;
    bpp_         = sinfo.bpp;
    xResolution_ = sinfo.x;
    yResolution_ = sinfo.y;
}

Screen::~Screen() {
}

void Screen::putPixel16(int x, int y, dword color) {

    int bytesPerPixel = bpp_ / 8;
    byte* vram       = vram_;

    vram += (x + y * xResolution_) * bytesPerPixel;
    *((word*)vram) = (word)color;
}

void Screen::fillRect16(int x, int y, int w, int h, dword color) {

        dword bytesPerPixel = bpp_ / 8;

        byte* position = vram_ + (x + y * xResolution_) * bytesPerPixel;
        byte* temp     = position;

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                *((word*)temp) = color;
                temp += bytesPerPixel;
            }
            position += xResolution_ * bytesPerPixel;
            temp = position;
        }
}

bool Screen::bitblt(Screen* destScreen, int destX, int destY, int width, int height
                    , Screen* sourceScreen, int sourceX, int sourceY) {

    /* check range */
    int maxX = destScreen->getWidth();
    int maxY = destScreen->getHeight();

    if (destX + width  > maxX) width  = maxX - destX;
    if (destY + height > maxY) height = maxY - destY;

    byte* dvram      = destScreen->getVRAM();
    byte* svram      = sourceScreen->getVRAM();
    int xResolution  = destScreen->getWidth();
    int bitsPerPixel = destScreen->getBpp();

    switch(bitsPerPixel) {

    case(16):
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                copyPixel16(dvram, destX + w, destY + h, svram, sourceX + w, sourceY + h, xResolution);
            }
        }
        break;

    case(24):
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {

                copyPixel24(dvram, destX + w, destY + h, svram, sourceX + w, sourceY + h, xResolution);
            }
        }
        break;

    case(32):

        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {

                copyPixel32(dvram, destX + w, destY + h, svram, sourceX + w, sourceY + h, xResolution);
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

bool Screen::bitblt(Screen* destScreen, int destX, int destY, int width, int height
                    , Screen* sourceScreen, int sourceX, int sourceY, int raster) {

    /* check range */
    int maxX = destScreen->getWidth();
    int maxY = destScreen->getHeight();

    if (destX + width  > maxX) width  = maxX - destX;
    if (destY + height > maxY) height = maxY - destY;

    byte* dvram      = destScreen->getVRAM();
    byte* svram      = sourceScreen->getVRAM();
    int xResolution  = destScreen->getWidth();
    int bitsPerPixel = destScreen->getBpp();

    switch(bitsPerPixel) {

    case(16):
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
                copyPixel16XOR(dvram, destX + w, destY + h, svram, sourceX + w, sourceY + h, xResolution);
            }
        }
        break;

    case(24):
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {

                copyPixel24XOR(dvram, destX + w, destY + h, svram, sourceX + w, sourceY + h, xResolution);
            }
        }
        break;

    case(32):
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {

                copyPixel32XOR(dvram, destX + w, destY + h, svram, sourceX + w, sourceY + h, xResolution);
            }
        }
        break;

    default:
        return false;
    }
    return true;
}

/* from orange pekoe */
void Screen::circle16(int x, int y, int r, dword color) {

    int w, h, f;
    w = r;
    h = 0;
    f = -2 * r + 3;

    while (w >= h) {
        putPixel16(x + w, y + h, color);
        putPixel16(x - w, y + h, color);
        putPixel16(x + h, y + w, color);
        putPixel16(x - h, y + w, color);
        putPixel16(x - w, y - h, color);
        putPixel16(x - h, y - w, color);
        putPixel16(x + h, y - w, color);
        putPixel16(x + w, y - h, color);

        if (f >= 0) {
            w--;
            f -= 4 * w;
        }
        h ++;
        f += 4 * h + 2;
    }
}

void Screen::fillCircle16(int x, int y, int r, dword color) {

    int i;
    int w, h;
    w = r;
    h = 0;
    int f = -2 * r + 3;

    while(w >= h) {
        for (i = x - w; i <= x + w; i ++) {
            putPixel16(i, y + h, color);
            putPixel16(i, y - h, color);
        }
        for (i = x - h; i <= x + h; i ++) {
            putPixel16(i, y - w, color);
            putPixel16(i, y + w, color);
        }
        if (f >= 0) {

            w--;
            f -= 4 * w;
        }
        h ++;
        f += 4 * h + 2;
    }
}

}

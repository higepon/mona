/*----------------------------------------------------------------------
    Screen
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Screen::Screen() {

    volatile ScreenInfo sinfo;

    /* get and set vram information */
    syscall_get_vram_info(&sinfo);
    vram_        = (uint8_t*)sinfo.vram;
    bpp_         = sinfo.bpp;
    xResolution_ = sinfo.x;
    yResolution_ = sinfo.y;
}

Screen::~Screen() {
}

void Screen::putPixel16(int x, int y, uint32_t color) {

    int uint8_tsPerPixel = bpp_ / 8;
    uint8_t* vram       = &vram_[(x + y * xResolution_) * uint8_tsPerPixel];
    uint8_t* p_color  = (uint8_t*)&color;

    switch (uint8_tsPerPixel)
    {
        case 2:
            *((uint16_t*)vram) = Color::bpp24to565(color);
            break;
        default:
            vram[0] = p_color[0];
            vram[1] = p_color[1];
            vram[2] = p_color[2];
            break;
    }
}

void Screen::fillRect16(int x, int y, int w, int h, uint32_t color) {

    uint32_t uint8_tsPerPixel = bpp_ / 8;

    uint8_t* position = &vram_[(x + y * xResolution_) * uint8_tsPerPixel];
    uint8_t* temp     = position;
    uint8_t* p_color  = (uint8_t*)&color;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            switch (uint8_tsPerPixel)
            {
                case 2:
                    *((uint16_t*)temp) = Color::bpp24to565(color);
                    break;
                default:
                    temp[0] = p_color[0];
                    temp[1] = p_color[1];
                    temp[2] = p_color[2];
                    break;
            }
            temp += uint8_tsPerPixel;
        }
        position += xResolution_ * uint8_tsPerPixel;
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

    uint8_t* dvram      = destScreen->getVRAM();
    uint8_t* svram      = sourceScreen->getVRAM();
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

    uint8_t* dvram      = destScreen->getVRAM();
    uint8_t* svram      = sourceScreen->getVRAM();
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
void Screen::circle16(int x, int y, int r, uint32_t color) {

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

void Screen::fillCircle16(int x, int y, int r, uint32_t color) {

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

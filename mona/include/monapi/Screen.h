#ifndef _MONAPI_SCREEN_
#define _MONAPI_SCREEN_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Raster
----------------------------------------------------------------------*/
class Raster {
  public:
    static const int XOR = 1;
};

/*----------------------------------------------------------------------
    Screen
----------------------------------------------------------------------*/
class Screen {

  public:
    Screen();
    virtual ~Screen();

  public:

    void circle16(int x, int y, int r, uint32_t color);
    void fillCircle16(int x, int y, int r, uint32_t color);
    void putPixel16(int x, int y, uint32_t color);
    void fillRect16(int x, int y, int w, int h, uint32_t color);
    static bool bitblt(Screen* destScreen, int destX, int destY, int width, int height
                , Screen* sourceScreen, int sourceX, int sourceY);
    static bool bitblt(Screen* destScreen, int destX, int destY, int width, int height
                , Screen* sourceScreen, int sourceX, int sourceY, int raster);

    inline uint8_t* getVRAM() const {
        return vram_;
    }

    inline int getBpp() const {
        return bpp_;
    }

    inline int getWidth() const {
        return xResolution_;
    }

    inline int getHeight() const {
        return yResolution_;
    }

   static inline void copyPixel16(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel24(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel32(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

   static inline void copyPixel16XOR(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;

        uint32_t dest  = destX + destY * xResolution;
        uint32_t source = sourceX + sourceY * xResolution;

        dpixel[dest].p[0] = (dpixel[dest].p[0]) ^ (spixel[source].p[0]);
        dpixel[dest].p[1] = (dpixel[dest].p[1]) ^ (spixel[source].p[1]);
    }

    static inline void copyPixel24XOR(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;

        uint32_t dest  = destX + destY * xResolution;
        uint32_t source = sourceX + sourceY * xResolution;

        dpixel[dest].p[0] = (dpixel[dest].p[0]) ^ (spixel[source].p[0]);
        dpixel[dest].p[1] = (dpixel[dest].p[1]) ^ (spixel[source].p[1]);
        dpixel[dest].p[2] = (dpixel[dest].p[2]) ^ (spixel[source].p[2]);
    }

    static inline void copyPixel32XOR(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;

        uint32_t dest  = destX + destY * xResolution;
        uint32_t source = sourceX + sourceY * xResolution;

        dpixel[dest].p = (dpixel[dest].p) ^ (spixel[source].p);
    }

  public:

  protected:
    uint8_t* vram_;
    int bpp_;
    int xResolution_;
    int yResolution_;
};

class VirtualScreen : public Screen {

  public:
    VirtualScreen();
    VirtualScreen(uint32_t vramsize);
    virtual ~VirtualScreen();

  protected:
};

}

#endif

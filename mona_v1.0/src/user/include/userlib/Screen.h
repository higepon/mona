#ifndef _MONA_USERLIB_SCREEN_
#define _MONA_USERLIB_SCREEN_

#include <types.h>

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

    void circle16(int x, int y, int r, dword color);
    void fillCircle16(int x, int y, int r, dword color);
    void putPixel16(int x, int y, dword color);
    void fillRect16(int x, int y, int w, int h, dword color);
    static bool bitblt(Screen* destScreen, int destX, int destY, int width, int height
                , Screen* sourceScreen, int sourceX, int sourceY);
    static bool bitblt(Screen* destScreen, int destX, int destY, int width, int height
                , Screen* sourceScreen, int sourceX, int sourceY, int raster);

    inline byte* getVRAM() const {
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

   static inline void copyPixel16(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel24(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel32(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

   static inline void copyPixel16XOR(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;

        dword dest  = destX + destY * xResolution;
        dword source = sourceX + sourceY * xResolution;

        dpixel[dest].p[0] = (dpixel[dest].p[0]) ^ (spixel[source].p[0]);
        dpixel[dest].p[1] = (dpixel[dest].p[1]) ^ (spixel[source].p[1]);
    }

    static inline void copyPixel24XOR(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;

        dword dest  = destX + destY * xResolution;
        dword source = sourceX + sourceY * xResolution;

        dpixel[dest].p[0] = (dpixel[dest].p[0]) ^ (spixel[source].p[0]);
        dpixel[dest].p[1] = (dpixel[dest].p[1]) ^ (spixel[source].p[1]);
        dpixel[dest].p[2] = (dpixel[dest].p[2]) ^ (spixel[source].p[2]);
    }

    static inline void copyPixel32XOR(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;

        dword dest  = destX + destY * xResolution;
        dword source = sourceX + sourceY * xResolution;

        dpixel[dest].p = (dpixel[dest].p) ^ (spixel[source].p);
    }

  public:

  protected:
    byte* vram_;
    int bpp_;
    int xResolution_;
    int yResolution_;
};

class VirtualScreen : public Screen {

  public:
    VirtualScreen();
    VirtualScreen(dword vramsize);
    virtual ~VirtualScreen();

  protected:
};
#endif

/*!
    \file  vbe.h
    \brief VESA

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/01/17 update:$Date$
*/

#ifndef __VBE_H__
#define __VBE_H__

struct VesaInfoDetail {
    word  bytesPerScanLine;         //8
    word  xResolution;              //9
    word  yResolution;              //10
    byte  bitsPerPixel;
    byte* physBasePtr;              //21 22
};

class kScreen {

  public:
    kScreen(int x, int y, byte bpp, byte* vram);
    virtual ~kScreen();

  public:
    inline int getXResolution() const {
        return xResolution_;
    }

    inline int getYResolution() const {
        return yResolution_;
    }

    inline int getBitsPerPixel() const {
        return bitsPerPixel_;
    }

    inline dword getVramSize() const {
        return vramSize_;
    }

    inline byte* getVram() const {
        return vram_;
    }

  private:

    static inline void copyPixel16(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel24(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel32(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel8(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel8* dpixel = (Pixel8*)dvram;
        Pixel8* spixel = (Pixel8*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

  public:
    static bool bitblt(kScreen* destScreen, int destX, int destY, int width, int height
                       , kScreen* sourceScreen, int sourceX, int sourceY, dword raster);

  private:
    int   xResolution_;
    int   yResolution_;
    byte  bitsPerPixel_;
    dword vramSize_;
    byte* vram_;
};

#endif

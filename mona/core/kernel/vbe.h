/*!
    \file  vbe.h
    \brief VESA

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/01/17 update:$Date$
*/

#ifndef _MONA_VBE_
#define _MONA_VBE_

#include <sys/types.h>

#pragma pack(2)
typedef struct VesaInfo {
    uint8_t  sign[4];
    uint8_t  versionL;
    uint8_t  versionH;
    uint32_t oemStringPtr;
    uint8_t  capabilities[4];
    uint32_t videoModePtr;
};
#pragma pack()

typedef struct VesaInfoDetail {
    uint16_t  modeAttributes;           //0
    uint8_t  winAAttributes;           //1
    uint8_t  winBAttributes;
    uint16_t  winGranularity;           //2
    uint16_t  winSize;                  //3
    uint16_t  winASegment;              //4
    uint16_t  winBSegment;              //5
    uint32_t winFuncPtr;               //67
    uint16_t  uint8_tsPerScanLine;         //8
    uint16_t  xResolution;              //9
    uint16_t  yResolution;              //10
    uint8_t  xCharSize;                //11
    uint8_t  yCharSize;
    uint8_t  numberOfPlanes;           //12
    uint8_t  bitsPerPixel;
    uint8_t  numberOfBanks;            //13
    uint8_t  memoryModel;
    uint8_t  bankSize;                 //14
    uint8_t  numberOfImagePages;
    uint8_t  reserved;                 //15
    uint8_t  redMaskSize;
    uint8_t  redFieldPosition;         //16
    uint8_t  greenMaskSize;
    uint8_t  greenFieldPosition;       //17
    uint8_t  blueMaskSize;
    uint8_t  blueFieldPosition;        //18
    uint8_t  rsvdMaskSize;
    uint8_t  rsvdFieldPos;             //20
    uint8_t  directColorModeInfo;
    uint32_t physBasePtr;              //21 22
};

class kScreen {

  public:
    kScreen(int x, int y, uint8_t bpp, uint8_t* vram);
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

    inline uint32_t getVramSize() const {
        return vramSize_;
    }

    inline uint8_t* getVram() const {
        return vram_;
    }

  private:

    static inline void copyPixel16(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel24(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel32(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel8(uint8_t* dvram, int destX, int destY, uint8_t* svram, int sourceX, int sourceY, int xResolution, int raster) {
        Pixel8* dpixel = (Pixel8*)dvram;
        Pixel8* spixel = (Pixel8*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

  public:
    static bool bitblt(kScreen* destScreen, int destX, int destY, int width, int height
                       , kScreen* sourceScreen, int sourceX, int sourceY, uint32_t raster);

  private:
    int   xResolution_;
    int   yResolution_;
    uint8_t  bitsPerPixel_;
    uint32_t vramSize_;
    uint8_t* vram_;
};

#endif

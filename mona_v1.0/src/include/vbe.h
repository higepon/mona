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

#include <types.h>

#pragma pack(2)
typedef struct VesaInfo {
    byte  sign[4];
    byte  versionL;
    byte  versionH;
    dword oemStringPtr;
    byte  capabilities[4];
    dword videoModePtr;
};
#pragma pack()

typedef struct VesaInfoDetail {
    word  modeAttributes;           //0
    byte  winAAttributes;           //1
    byte  winBAttributes;
    word  winGranularity;           //2
    word  winSize;                  //3
    word  winASegment;              //4
    word  winBSegment;              //5
    dword winFuncPtr;               //67
    word  bytesPerScanLine;         //8
    word  xResolution;              //9
    word  yResolution;              //10
    byte  xCharSize;                //11
    byte  yCharSize;
    byte  numberOfPlanes;           //12
    byte  bitsPerPixel;
    byte  numberOfBanks;            //13
    byte  memoryModel;
    byte  bankSize;                 //14
    byte  numberOfImagePages;
    byte  reserved;                 //15
    byte  redMaskSize;
    byte  redFieldPosition;         //16
    byte  greenMaskSize;
    byte  greenFieldPosition;       //17
    byte  blueMaskSize;
    byte  blueFieldPosition;        //18
    byte  rsvdMaskSize;
    byte  rsvdFieldPos;             //20
    byte  directColorModeInfo;
    dword physBasePtr;              //21 22
};

typedef struct Pixel16 {
    byte dummy[2];
};


class Screen {

  public:
    Screen(int x, int y, byte bpp, byte* vram);
    virtual ~Screen();

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

  public:
    static bool bitblt16(Screen* destScreen, int destX, int destY, int width, int height
                       , Screen* sourceScreen, int sourceX, int sourceY, dword raster);

  private:
    int   xResolution_;
    int   yResolution_;
    byte  bitsPerPixel_;
    dword vramSize_;
    byte* vram_;
};

#endif

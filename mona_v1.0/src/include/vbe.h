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
    word  modeAttributes;
    byte  winAAttributes;
    byte  winBAttributes;
    word  winGranularity;
    word  winSize;
    word  winASegment;
    word  winBSegment;
    dword winFuncPtr;
    word  bytesPerScanLine;
    word  xResolution;
    word  yResolution;
    byte  xCharSize;
    byte  yCharSize;
    byte  numberOfPlanes;
    byte  bitsPerPixel;
    byte  numberOfBanks;
    byte  memoryModel;
    byte  bankSize;
    byte  numberOfImagePages;
    byte  reserved;
    byte  redMaskSize;
    byte  redFieldPosition;
    byte  greenMaskSize;
    byte  greenFieldPosition;
    byte  blueMaskSize;
    byte  blueFieldPosition;
    byte  rsvdMaskSize;
    byte  directColorModeInfo;
};

#endif

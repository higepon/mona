/*!
    \file  GDTUtil.h
    \brief some functions for GDT set up

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/06 update:$Date$
*/
#ifndef _MONA_GDTUTIL_
#define _MONA_GDTUTIL_

#define GDT_ENTRY_NUM 8192
#define GDT_SIZE      (GDT_ENTRY_NUM * 8)

#define SEGMENT_ABSENT  0x00
#define SEGMENT_PRESENT 0x80
#define SEGMENT_DPL0    0x00
#define SEGMENT_DPL1    0x20
#define SEGMENT_DPL2    0x40
#define SEGMENT_DPL3    0x60

/*!
    \struct  segment descripter
*/
typedef struct {
    word limitL;
    word baseL;
    byte baseM;
    byte type;
    byte limitH;
    byte baseH;
} SegDesc;

#pragma pack(2)

/*!
    \struct gdtr
*/
typedef struct {
    word  limit;
    dword base;
} GDTR;

#pragma pack()

class GDTUtil {

  public:
    static void setSegDesc(SegDesc* desc, dword base, dword limit, byte type);
    static void lgdt();
    static void setUpGDT();
};

#endif

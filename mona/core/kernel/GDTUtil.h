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

#define GDT_ENTRY_SELECTOR(n) (n * sizeof(SegDesc))
#define KERNEL_CS GDT_ENTRY_SELECTOR(1)
#define KERNEL_DS GDT_ENTRY_SELECTOR(2)
#define KERNEL_SS GDT_ENTRY_SELECTOR(3)
#define USER_CS   GDT_ENTRY_SELECTOR(5) | DPL_USER
#define USER_DS   GDT_ENTRY_SELECTOR(6) | DPL_USER
#define USER_SS   GDT_ENTRY_SELECTOR(7) | DPL_USER

/*!
    \struct SegDesc
*/
typedef struct {
    uint16_t limitL;
    uint16_t baseL;
    uint8_t baseM;
    uint8_t type;
    uint8_t limitH;
    uint8_t baseH;
} SegDesc;

#pragma pack(2)

/*!
    \struct gdtr
*/
typedef struct {
    uint16_t  limit;
    uint32_t base;
} GDTR;

#pragma pack()

class GDTUtil {

  public:
    static void setSegDesc(SegDesc* desc, uint32_t base, uint32_t limit, uint8_t type);
    static void setSegDescExt(SegDesc* desc, uint32_t base, uint32_t limit, uint8_t type, uint8_t gdbavl);
    static void lgdt(GDTR* gdtr);
    static void ltr(uint16_t selector) __attribute__ ((noinline)); /* we need this! */
    static void setup();
    static void setupTSS(uint16_t selector);
};

#endif

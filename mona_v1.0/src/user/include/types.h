/*!
    \file  types.h
    \brief types defineition

    types defintion
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/09/08 update:$Date$
*/

#ifndef _MONA_TYPES_
#define _MONA_TYPES_

#define NULL     0
#define NORMAL   0
#define DEBUG_MODE

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;

typedef dword linear_addr;  /* 32bit */
typedef dword virtual_addr; /* 32bit */
typedef dword phys_addr;    /* 32bit */

typedef struct {
    dword header;
    dword arg1;
    dword arg2;
    dword arg3;
    dword from;
    char str[128];
    int length;
} MessageInfo;

typedef struct {
    dword vram;
    dword bpp;
    dword x;
    dword y;
} ScreenInfo;

typedef struct Pixel16 {
    byte p[2];
};

typedef struct Pixel24 {
    byte p[3];
};

typedef struct Pixel32 {
    dword p;
};

typedef struct Pixel8 {
    byte p;
};

typedef struct MappingInfo {
    dword attachPid;
    dword linearAddress1;
    dword linearAddress2;
    dword size;
    dword errorCd;
};


#define MSG_KEY_SCANCODE           0x00
#define MSG_KEY_REGIST_TO_SERVER   0x01
#define MSG_MAP                    0x02
#define MSG_RESULT_OK              0x03
#define MSG_RESULT_ERROR           0x04
#define MSG_KEY_VIRTUAL_CODE       0x05
#define MSG_MOUSE_1                0x06
#define MSG_MOUSE_2                0x07
#define MSG_MOUSE_3                0x08
#define MSG_MOUSE_REGIST_TO_SERVER 0x09
#define MSG_MOUSE_INFO             0x0A

#define SHARED_FDC_BUFFER 0x4000

#endif

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

#define MSG_KEY_SCANCODE         0x00
#define MSG_KEY_REGIST_TO_SERVER 0x01
#define MSG_MAP                  0x02
#define MSG_RESULT_OK            0x03
#define MSG_RESULT_ERROR         0x04

#define SHARED_FDC_BUFFER 0x4000

#endif

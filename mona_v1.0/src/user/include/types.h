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
} MessageInfo;

#define MSG_KEY_SCANCODE         0x00
#define MSG_KEY_REGIST_TO_SERVER 0x01

#endif

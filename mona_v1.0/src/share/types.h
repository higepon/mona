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
#define interface class

typedef dword linear_addr;  /* 32bit */
typedef dword virtual_addr; /* 32bit */
typedef dword phys_addr;    /* 32bit */

typedef struct {
    int year;
    int month;
    int day;
    int dayofweek;
    int hour;
    int min;
    int sec;
} KDate;

struct CommandOption {
    char str[32];
    struct CommandOption* next;
};
typedef struct CommandOption CommandOption;

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

#define WAIT_MESSAGE 0x00
#define WAIT_NONE    0xFF


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

#define SYSTEM_CALL_PROCESS_SLEEP   5
#define SYSTEM_CALL_HEAVEY          6
#define SYSTEM_CALL_PRINT           7
#define SYSTEM_CALL_KILL            8
#define SYSTEM_CALL_PUT_PIXEL       9
#define SYSTEM_CALL_SEND            10
#define SYSTEM_CALL_RECEIVE         11
#define SYSTEM_CALL_MTHREAD_CREATE  12
#define SYSTEM_CALL_MTHREAD_JOIN    13
#define SYSTEM_CALL_MUTEX_CREATE    14
#define SYSTEM_CALL_MUTEX_LOCK      15
#define SYSTEM_CALL_MUTEX_TRYLOCK   16
#define SYSTEM_CALL_MUTEX_UNLOCK    17
#define SYSTEM_CALL_MUTEX_DESTROY   18
#define SYSTEM_CALL_LOOKUP          19
#define SYSTEM_CALL_GET_VRAM_INFO   20
#define SYSTEM_CALL_LOAD_PROCESS    21
#define SYSTEM_CALL_MAP             22
#define SYSTEM_CALL_SET_CURSOR      23
#define SYSTEM_CALL_GET_CURSOR      24
#define SYSTEM_CALL_FILE_OPEN       25
#define SYSTEM_CALL_FILE_READ       26
#define SYSTEM_CALL_FILE_CLOSE      27
#define SYSTEM_CALL_MAP_TWO         28
#define SYSTEM_CALL_FDC_OPEN        29
#define SYSTEM_CALL_FDC_CLOSE       30
#define SYSTEM_CALL_FDC_READ        31
#define SYSTEM_CALL_FDC_WRITE       32
#define SYSTEM_CALL_UNMAP_TWO       33
#define SYSTEM_CALL_GET_PID         34
#define SYSTEM_CALL_ARGUMENTS_NUM   35
#define SYSTEM_CALL_GET_ARGUMENTS   36
#define SYSTEM_CALL_MTHREAD_YIELD_M 37
#define SYSTEM_CALL_DATE            38
#define SYSTEM_CALL_GET_IO          39
#define SYSTEM_CALL_TEST            99

#endif

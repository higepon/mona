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

/* temp!! remove later */
#define MOUSE_SERVER    10
#define SHELL_SERVER    20
#define KEYBOARD_SERVER 30


#define NULL     0
#define NORMAL   0
#define DEBUG_MODE

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;
typedef dword          kevent;
#define interface class

#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif

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

#define WAIT_TIMER   0x60
#define WAIT_NONE    0xFF


#define MSG_KEY_SCANCODE               0x00
#define MSG_KEY_REGIST_TO_SERVER       0x01
#define MSG_MAP                        0x02
#define MSG_RESULT_OK                  0x03
#define MSG_RESULT_ERROR               0x04
#define MSG_KEY_VIRTUAL_CODE           0x05
#define MSG_MOUSE_1                    0x06
#define MSG_MOUSE_2                    0x07
#define MSG_MOUSE_3                    0x08
#define MSG_MOUSE_REGIST_TO_SERVER     0x09
#define MSG_MOUSE_INFO                 0x0A
#define MSG_MOUSE_UNREGIST_FROM_SERVER 0x0B
#define MSG_KEY_UNREGIST_FROM_SERVER   0x0C

#define SHARED_FDC_BUFFER 0x4000

#define SYSTEM_CALL_THREAD_SLEEP          5
#define SYSTEM_CALL_HEAVEY                6
#define SYSTEM_CALL_PRINT                 7
#define SYSTEM_CALL_KILL                  8
#define SYSTEM_CALL_PUT_PIXEL             9
#define SYSTEM_CALL_SEND                  10
#define SYSTEM_CALL_RECEIVE               11
#define SYSTEM_CALL_MTHREAD_CREATE        12
#define SYSTEM_CALL_MTHREAD_JOIN          13
#define SYSTEM_CALL_MUTEX_CREATE          14
#define SYSTEM_CALL_MUTEX_LOCK            15
#define SYSTEM_CALL_MUTEX_TRYLOCK         16
#define SYSTEM_CALL_MUTEX_UNLOCK          17
#define SYSTEM_CALL_MUTEX_DESTROY         18
#define SYSTEM_CALL_LOOKUP                19
#define SYSTEM_CALL_GET_VRAM_INFO         20
#define SYSTEM_CALL_LOAD_PROCESS          21
#define SYSTEM_CALL_MAP                   22
#define SYSTEM_CALL_SET_CURSOR            23
#define SYSTEM_CALL_GET_CURSOR            24
#define SYSTEM_CALL_FILE_OPEN             25
#define SYSTEM_CALL_FILE_READ             26
#define SYSTEM_CALL_FILE_CLOSE            27
#define SYSTEM_CALL_MAP_TWO               28
#define SYSTEM_CALL_FDC_OPEN              29
#define SYSTEM_CALL_FDC_CLOSE             30
#define SYSTEM_CALL_FDC_READ              31
#define SYSTEM_CALL_FDC_WRITE             32
#define SYSTEM_CALL_UNMAP_TWO             33
#define SYSTEM_CALL_GET_PID               34
#define SYSTEM_CALL_ARGUMENTS_NUM         35
#define SYSTEM_CALL_GET_ARGUMENTS         36
#define SYSTEM_CALL_MTHREAD_YIELD_MESSAGE 37
#define SYSTEM_CALL_DATE                  38
#define SYSTEM_CALL_GET_IO                39
#define SYSTEM_CALL_WAIT_FDC              40
#define SYSTEM_CALL_EXIST_MESSAGE         41
#define SYSTEM_CALL_FDC_DISK_CHANGED      42
#define SYSTEM_CALL_GET_TID               43
#define SYSTEM_CALL_TEST                  99

#define SYSCALL_0(syscall_number, result)                                         \
    asm volatile("movl $%c1, %%ebx \n"                                            \
                 "int  $0x80       \n"                                            \
                 "movl %%eax, %0   \n"                                            \
                 :"=m"(result)                                                    \
                 :"g"(syscall_number)                                             \
                 :"ebx"                                                           \
                 );                                                               \

#define SYSCALL_1(syscall_number, result, arg1)                                   \
    asm volatile("movl $%c1, %%ebx \n"                                            \
                 "movl %2  , %%esi \n"                                            \
                 "int  $0x80       \n"                                            \
                 "movl %%eax, %0   \n"                                            \
                 :"=m"(result)                                                    \
                 :"g"(syscall_number), "m"(arg1)                                  \
                 :"ebx", "esi"                                                    \
                 );                                                               \

#define SYSCALL_2(syscall_number, result, arg1, arg2)                             \
    asm volatile("movl $%c1, %%ebx \n"                                            \
                 "movl %2  , %%esi \n"                                            \
                 "movl %3  , %%ecx \n"                                            \
                 "int  $0x80       \n"                                            \
                 "movl %%eax, %0   \n"                                            \
                 :"=m"(result)                                                    \
                 :"g"(syscall_number), "m"(arg1), "m"(arg2)                       \
                 :"ebx", "esi", "ecx"                                             \
                 );                                                               \

#define SYSCALL_3(syscall_number, result, arg1, arg2, arg3)                       \
    asm volatile("movl $%c1, %%ebx \n"                                            \
                 "movl %2  , %%esi \n"                                            \
                 "movl %3  , %%ecx \n"                                            \
                 "movl %4  , %%edi \n"                                            \
                 "int  $0x80       \n"                                            \
                 "movl %%eax, %0   \n"                                            \
                 :"=m"(result)                                                    \
                 :"g"(syscall_number), "m"(arg1), "m"(arg2), "m"(arg3)            \
                 :"ebx", "esi", "ecx", "edi"                                      \
                 );                                                               \

#define SYSCALL_4(syscall_number, result, arg1, arg2, arg3, arg4)                 \
    asm volatile("movl $%c1, %%ebx \n"                                            \
                 "movl %2  , %%esi \n"                                            \
                 "movl %3  , %%ecx \n"                                            \
                 "movl %4  , %%edi \n"                                            \
                 "movl %5  , %%edx \n"                                            \
                 "int  $0x80       \n"                                            \
                 "movl %%eax, %0   \n"                                            \
                 :"=m"(result)                                                    \
                 :"g"(syscall_number), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4) \
                 :"ebx", "esi", "ecx", "edi", "edx"                               \
                 );                                                               \

#endif

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

//#define DEBUG_READ_TRACE

#define NULL     0
#define NORMAL   0
#define DEBUG_MODE

enum {
    MONA_SUCCESS = 1, /* don't change */
    MONA_FAILURE = 0  /* don't change */
};

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ size_t;

#ifndef __uint8_t_defined
typedef unsigned char       uint8_t;
# define __uint8_t_defined
#endif

#ifndef __uint16_t_defined
typedef unsigned short      uint16_t;
# define __uint16_t_defined
#endif
#ifndef __uint32_t_defined
typedef unsigned int            uint32_t;
# define __uint32_t_defined
#endif

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

// should be less than MessageInfo.str
#define MAX_PROCESS_ARGUMENT_LENGTH 64

typedef dword linear_addr;  /* 32bit */
typedef dword virtual_addr; /* 32bit */
typedef dword phys_addr;    /* 32bit */

typedef struct
{
    dword totalMemoryH;
    dword totalMemoryL;
    dword freePageNum;
    dword totalPageNum;
    dword pageSize;
} MemoryInfo;

typedef struct PsInfo {
    char name[16];
    dword state;
    dword cr3;
    dword eip;
    dword esp;
    dword tid;
    struct PsInfo* next;
} PsInfo;

typedef struct
{
    dword size;
} Stat;

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
    char str[MAX_PROCESS_ARGUMENT_LENGTH];
    struct CommandOption* next;
};
typedef struct CommandOption CommandOption;

typedef struct {
    byte* image;
    dword size;
    dword entrypoint;
    const char* path;
    const char* name;
    CommandOption* list;
} LoadProcessInfo;


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

typedef struct {
    byte p[2];
} Pixel16;

typedef struct {
    byte p[3];
} Pixel24;

typedef struct {
    dword p;
} Pixel32;

typedef struct {
    byte p;
} Pixel8;

typedef struct {
    dword attachPid;
    dword linearAddress1;
    dword linearAddress2;
    dword size;
    dword errorCd;
} MappingInfo;

typedef struct
{
    dword arg1;
    dword arg2;
    dword arg3;
    dword arg4;
} SysArg;

/*!
    struct for key information
*/
typedef struct {
    unsigned int keycode;
    unsigned int modifiers;
    unsigned int charcode;
} KeyInfo;

#define WAIT_TIMER   0x60
#define WAIT_NONE    0xFF

#define THREAD_UNKNOWN 0xffffffff

enum
{
    MSG_NONE = 0,
    MSG_MAP,
    MSG_MEMORY_MAP_ID,
    MSG_RESULT_OK,
    MSG_RESULT_ERROR,
    MSG_SERVER_START_OK,
    MSG_INTERRUPTED,
    MSG_MONES_REGIST,
    MSG_MONES_IP_SEND,
    MSG_MONES_IP_NOTICE,
    MSG_MONES_ICMP_SEND,
    MSG_MONES_ICMP_NOTICE,
    MSG_MONES_FRAME_REQ,
    MSG_MONES_ARPTBL_REQ,
    MSG_MONES_WAKEUP_ARP_WAIT,
    MSG_THREAD_KILLED = 0x800,
    MSG_TIMER,
    MSG_READ_MEMORY_READY,
    MSG_WRITE_MEMORY_READY,
    MSG_STREAM_ID,
};

enum
{
    ATTRIBUTE_DIRECTORY = 0x10
};

enum
{
    PEEK_REMOVE = 0x01
};

enum
{
    DRIVE_NONE = -1,
    DRIVE_FD0  = 0,
    DRIVE_CD0  = 1,
};

enum
{
        SHUTDOWN_HALT,
        SHUTDOWN_REBOOT,
        SHUTDOWN_STANDBY,
        SHUTDOWN_SUSPEND,
        SHUTDOWN_FEATURE,
        SHUTDOWN_FEATURE_APM = 0x0001,

        SHUTDOWN_DEVICE_ALL = 0x0001,
        SHUTDOWN_DEVICE_DISPLAY_ALL = 0x01FF,
};

#define SHARED_FDC_BUFFER 0x4000

#define SYSTEM_CALL_PRINT                    0x0001
#define SYSTEM_CALL_KILL                     0x0002
#define SYSTEM_CALL_SEND                     0x0003
#define SYSTEM_CALL_RECEIVE                  0x0004
#define SYSTEM_CALL_EXIST_MESSAGE            0x0005
#define SYSTEM_CALL_MTHREAD_CREATE           0x0006
#define SYSTEM_CALL_MTHREAD_JOIN             0x0007
#define SYSTEM_CALL_MTHREAD_SLEEP            0x0008
#define SYSTEM_CALL_MTHREAD_YIELD_MESSAGE    0x0009
#define SYSTEM_CALL_MUTEX_CREATE             0x000A
#define SYSTEM_CALL_MUTEX_LOCK               0x000B
#define SYSTEM_CALL_MUTEX_TRYLOCK            0x000C
#define SYSTEM_CALL_MUTEX_UNLOCK             0x000D
#define SYSTEM_CALL_MUTEX_DESTROY            0x000E
#define SYSTEM_CALL_LOOKUP                   0x000F
#define SYSTEM_CALL_LOOKUP_MAIN_THREAD       0x0010
#define SYSTEM_CALL_GET_VRAM_INFO            0x0011
#define SYSTEM_CALL_LOAD_PROCESS             0x0012
#define SYSTEM_CALL_SET_CURSOR               0x0013
#define SYSTEM_CALL_GET_CURSOR               0x0014
#define SYSTEM_CALL_FILE_OPEN                0x0015
#define SYSTEM_CALL_FILE_READ                0x0016
#define SYSTEM_CALL_FILE_WRITE               0x0017
#define SYSTEM_CALL_FILE_CLOSE               0x0018
#define SYSTEM_CALL_FDC_OPEN                 0x0019
#define SYSTEM_CALL_FDC_CLOSE                0x001A
#define SYSTEM_CALL_FDC_READ                 0x001B
#define SYSTEM_CALL_FDC_WRITE                0x001C
#define SYSTEM_CALL_FDC_DISK_CHANGED         0x001D
#define SYSTEM_CALL_WAIT_FDC                 0x001E
#define SYSTEM_CALL_GET_PID                  0x001F
#define SYSTEM_CALL_GET_TID                  0x0020
#define SYSTEM_CALL_ARGUMENTS_NUM            0x0021
#define SYSTEM_CALL_GET_ARGUMENTS            0x0022
#define SYSTEM_CALL_DATE                     0x0023
#define SYSTEM_CALL_GET_IO                   0x0024
#define SYSTEM_CALL_MEMORY_MAP_CREATE        0x0025
#define SYSTEM_CALL_MEMORY_MAP_GET_SIZE      0x0026
#define SYSTEM_CALL_MEMORY_MAP_MAP           0x0027
#define SYSTEM_CALL_MEMORY_MAP_UNMAP         0x0028
#define SYSTEM_CALL_FILE_CREATE              0x0029
#define SYSTEM_CALL_DIR_OPEN                 0x002A
#define SYSTEM_CALL_DIR_CLOSE                0x002B
#define SYSTEM_CALL_DIR_READ                 0x002C
#define SYSTEM_CALL_CD                       0x002D
#define SYSTEM_CALL_PS_DUMP_SET              0x002E
#define SYSTEM_CALL_PS_DUMP_READ             0x002F
#define SYSTEM_CALL_GET_TICK                 0x0030
#define SYSTEM_CALL_FILE_SEEK                0x0031
#define SYSTEM_CALL_FILE_POSITION            0x0032
#define SYSTEM_CALL_GET_KERNEL_VERSION       0x0033
#define SYSTEM_CALL_LOAD_PROCESS_IMAGE       0x0034
#define SYSTEM_CALL_KILL_THREAD              0x0035
#define SYSTEM_CALL_CLEAR_SCREEN             0x0036
#define SYSTEM_CALL_PEEK                     0x0037
#define SYSTEM_CALL_TEST                     0x0038
#define SYSTEM_CALL_SET_IRQ_RECEIVER         0x0039
#define SYSTEM_CALL_REMOVE_IRQ_RECEIVER      0x003A
#define SYSTEM_CALL_FREE_PAGES               0x003B
#define SYSTEM_CALL_GET_MEMORY_INFO          0x003C
#define SYSTEM_CALL_LOG_PRINT                0x003D
#define SYSTEM_CALL_ALLOCATE_DMA_MEMORY      0x003E
#define SYSTEM_CALL_DEALLOCATE_DMA_MEMORY    0x003F
#define SYSTEM_CALL_SET_TIMER                0x0040
#define SYSTEM_CALL_KILL_TIMER               0x0041
#define SYSTEM_CALL_HAS_IRQ_RECEIVER         0x0042
#define SYSTEM_CALL_CHANGE_BASE_PRIORITY     0x0043
#define SYSTEM_CALL_SET_DLL_SEGMENT_WRITABLE 0x0044
#define SYSTEM_CALL_SET_DLL_SEGMENT_NOTSHARED 0x0045
#define SYSTEM_CALL_SHUTDOWN                 0x0046
#define SYSTEM_CALL_APM_BIOS                 0x0047


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

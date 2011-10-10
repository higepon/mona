/*!
    \file  types.h
    \brief types defineition

    types defintion
    Copyright (c) 2002 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2002/09/08 update:$Date$
*/

#ifndef _MONA_TYPES_
#define _MONA_TYPES_

/* #define DEBUG_READ_TRACE */

#define MAP_PAGE_SIZE 4096

#define NULL     0
#define DEBUG_MODE

#define DLL_PROCESS_ATTACH 0
#define DLL_PROCESS_DETACH 1

#define KERNEL_TIMER_INTERVAL_MSEC 10

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef __uint64_t_defined
typedef unsigned long long	uint64_t;
# define __uint64_t_defined
#endif
#ifndef __int64_t_defined
typedef long long	int64_t;
# define __int64_t_defined
#endif

#define PROCESS_HEAP_START 0xc0000000
#define PROCESS_HEAP_SIZE 512 * 1024 * 1024

#ifndef __uintptr_t_defined
typedef unsigned long int   uintptr_t;
typedef long int        intptr_t;
# define __uintptr_t_defined
#endif

typedef intptr_t mutex_t;
typedef intptr_t cond_t;
typedef uintptr_t off_t;
typedef intptr_t ssize_t;

enum {
    /* Each value should be unique, it is used for identifing events on Scheduler. */
    M_EVENT_NONE             = 64,
    M_EVENT_SLEEP            = 20,
    M_EVENT_TIMER_MESSAGE    = 19,
    M_EVENT_MESSAGE          = 22,
    M_EVENT_MUTEX_UNLOCKED   = 27,
    M_EVENT_CONDITION_NOTIFY = 29,
    M_EVENT_SEMAPHORE_UPPED  = 28,
    M_EVENT_INTERRUPT_HIGH   = 0,
    M_EVENT_INTERRUPT_MIDDLE = 7,
    M_EVENT_INTERRUPT_LOW    = 17
};

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
#ifndef __size_t_defined
#define __size_t_defined
typedef __SIZE_TYPE__ size_t;
#endif

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
typedef unsigned int __uint32_t;
# define __uint32_t_defined
#endif

#ifndef __int32_t_defined
typedef signed int            int32_t;
typedef signed int __int32_t;
# define __int32_t_defined
#endif

#ifndef __uintptr_t_defined
typedef unsigned long int   uintptr_t;
# define __uintptr_t_defined
#endif

#define interface class

#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif

/* Independent from length of MessageInfo.str. */
#define MAX_PROCESS_ARGUMENT_LENGTH 256

typedef uint32_t linear_addr;  /* 32bit */
typedef uint32_t virtual_addr; /* 32bit */
typedef uint32_t phys_addr;    /* 32bit */

typedef struct
{
    uint32_t totalMemoryH;
    uint32_t totalMemoryL;
    uint32_t freePageNum;
    uint32_t totalPageNum;
    uint32_t pageSize;
} MemoryInfo;

typedef struct PsInfo {
    char name[16];
    uint32_t state;
    uint32_t cr3;
    uint32_t eip;
    uint32_t esp;
    uint32_t tid;
    struct PsInfo* next;
} PsInfo;

typedef struct {
    int year;
    int month;
    int day;
    int dayofweek;
    int hour;
    int min;
    int sec;
} KDate;

typedef struct
{
    uint32_t size;
    KDate datetime;
} Stat;

struct CommandOption {
    char str[MAX_PROCESS_ARGUMENT_LENGTH];
    struct CommandOption* next;
};
typedef struct CommandOption CommandOption;

typedef struct {
    uint8_t* image;
    uint32_t size;
    uint32_t entrypoint;
    const char* path;
    const char* name;
    CommandOption* list;
    uint32_t observer;
    uint32_t tid;
} LoadProcessInfo;

#define MESSAGE_INFO_MAX_STR_LENGTH 256
#define MAX_MESSAGES 1024

typedef struct {
    uintptr_t header;
    uintptr_t arg1;
    uintptr_t arg2;
    uintptr_t arg3;
    uintptr_t from;
    char str[MESSAGE_INFO_MAX_STR_LENGTH];
    int length;
} MessageInfo;

typedef struct {
    uint32_t vram;
    uint32_t bpp;
    uint32_t x;
    uint32_t y;
} ScreenInfo;

typedef struct {
    uint8_t p[2];
} Pixel16;

typedef struct {
    uint8_t p[3];
} Pixel24;

typedef struct {
    uint32_t p;
} Pixel32;

typedef struct {
    uint8_t p;
} Pixel8;

typedef struct {
    uint32_t attachPid;
    uint32_t linearAddress1;
    uint32_t linearAddress2;
    uint32_t size;
    uint32_t errorCd;
} MappingInfo;

typedef struct
{
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    uint32_t arg4;
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

/* common messages */
#define MSG_OK                0xfffff72a /* "   : OK" */
#define MSG_STARTED           0xffff29c6 /* "   :STR" */
#define MSG_INTERRUPTED       0xfffe86ce /* "   :INT" */
#define MSG_TIMER             0xffff3646 /* "   :TMR" */
#define MSG_READ_READY        0xffff11e2 /* "   :RDY" */
#define MSG_WRITE_READY       0xffff61e2 /* "   :WDY" */
#define MSG_TEXT              0xffff3bce /* "   :TXT" */
#define MSG_ADD               0xfffe018e /* "   :ADD" */
#define MSG_REMOVE            0xffff1656 /* "   :RMV" */
#define MSG_NAME              0xfffff6b2 /* "   : NM" */
#define MSG_WHERE             0xffff63c6 /* "   :WHR" */
#define MSG_STOP              0xffff29be /* "   :STP" */
#define MSG_UPDATE            0xffff478e /* "   :UPD" */
#define MSG_TEST_CLEAR_INPUT  0x9ca625a2 /* "TST:CLI" */
#define MSG_TEST_CLEAR_OUTPUT 0x9ca625ba /* "TST:CLO" */

enum
{
    ATTRIBUTE_DIRECTORY = 0x10
};

enum
{
    MUTEX_CREATE_NEW = 0
};

enum
{
    PEEK_REMOVE = 0x01
};

enum
{
    DRIVE_NONE = -1,
    DRIVE_FD0  = 0,
    DRIVE_CD0  = 1
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
        SHUTDOWN_DEVICE_DISPLAY_ALL = 0x01FF
};

#define SHARED_FDC_BUFFER 0x4000

#define SYSTEM_CALL_PRINT                     0x0001
#define SYSTEM_CALL_KILL                      0x0002
#define SYSTEM_CALL_SEND                      0x0003
#define SYSTEM_CALL_RECEIVE                   0x0004
#define SYSTEM_CALL_EXIST_MESSAGE             0x0005
#define SYSTEM_CALL_MTHREAD_CREATE            0x0006
/* #define SYSTEM_CALL_MTHREAD_JOIN              0x0007  */ /* not used */
#define SYSTEM_CALL_MTHREAD_SLEEP             0x0008
#define SYSTEM_CALL_MTHREAD_YIELD_MESSAGE     0x0009
#define SYSTEM_CALL_MUTEX_CREATE              0x000A
#define SYSTEM_CALL_MUTEX_LOCK                0x000B
#define SYSTEM_CALL_MUTEX_TRY_LOCK            0x000C
#define SYSTEM_CALL_MUTEX_UNLOCK              0x000D
#define SYSTEM_CALL_MUTEX_DESTROY             0x000E
#define SYSTEM_CALL_LOOKUP                    0x000F
#define SYSTEM_CALL_LOOKUP_MAIN_THREAD        0x0010
#define SYSTEM_CALL_GET_VRAM_INFO             0x0011
#define SYSTEM_CALL_LOAD_PROCESS              0x0012
#define SYSTEM_CALL_SET_CURSOR                0x0013
#define SYSTEM_CALL_GET_CURSOR                0x0014
#define SYSTEM_CALL_FILE_OPEN                 0x0015
#define SYSTEM_CALL_FILE_READ                 0x0016
#define SYSTEM_CALL_FILE_WRITE                0x0017
#define SYSTEM_CALL_FILE_CLOSE                0x0018
#define SYSTEM_CALL_FDC_OPEN                  0x0019
#define SYSTEM_CALL_FDC_CLOSE                 0x001A
#define SYSTEM_CALL_FDC_READ                  0x001B
#define SYSTEM_CALL_FDC_WRITE                 0x001C
#define SYSTEM_CALL_FDC_DISK_CHANGED          0x001D
#define SYSTEM_CALL_WAIT_FDC                  0x001E
#define SYSTEM_CALL_GET_PID                   0x001F
#define SYSTEM_CALL_GET_TID                   0x0020
#define SYSTEM_CALL_ARGUMENTS_NUM             0x0021
#define SYSTEM_CALL_GET_ARGUMENTS             0x0022
#define SYSTEM_CALL_DATE                      0x0023
#define SYSTEM_CALL_GET_IO                    0x0024
#define SYSTEM_CALL_MEMORY_MAP_CREATE         0x0025
#define SYSTEM_CALL_MEMORY_MAP_GET_SIZE       0x0026
#define SYSTEM_CALL_MEMORY_MAP_MAP            0x0027
#define SYSTEM_CALL_MEMORY_MAP_UNMAP          0x0028
#define SYSTEM_CALL_FILE_CREATE               0x0029
#define SYSTEM_CALL_DIR_OPEN                  0x002A
#define SYSTEM_CALL_DIR_CLOSE                 0x002B
#define SYSTEM_CALL_DIR_READ                  0x002C
#define SYSTEM_CALL_CD                        0x002D
#define SYSTEM_CALL_PS_DUMP_SET               0x002E
#define SYSTEM_CALL_PS_DUMP_READ              0x002F
#define SYSTEM_CALL_GET_TICK                  0x0030
#define SYSTEM_CALL_FILE_SEEK                 0x0031
#define SYSTEM_CALL_FILE_POSITION             0x0032
#define SYSTEM_CALL_GET_KERNEL_VERSION        0x0033
#define SYSTEM_CALL_LOAD_PROCESS_IMAGE        0x0034
#define SYSTEM_CALL_KILL_THREAD               0x0035
#define SYSTEM_CALL_CLEAR_SCREEN              0x0036
#define SYSTEM_CALL_PEEK                      0x0037
#define SYSTEM_CALL_TEST                      0x0038
#define SYSTEM_CALL_SET_IRQ_RECEIVER          0x0039
#define SYSTEM_CALL_REMOVE_IRQ_RECEIVER       0x003A
#define SYSTEM_CALL_FREE_PAGES                0x003B
#define SYSTEM_CALL_GET_MEMORY_INFO           0x003C
#define SYSTEM_CALL_LOG_PRINT                 0x003D
#define SYSTEM_CALL_ALLOCATE_DMA_MEMORY       0x003E
#define SYSTEM_CALL_DEALLOCATE_DMA_MEMORY     0x003F
#define SYSTEM_CALL_SET_TIMER                 0x0040
#define SYSTEM_CALL_KILL_TIMER                0x0041
#define SYSTEM_CALL_HAS_IRQ_RECEIVER          0x0042
#define SYSTEM_CALL_CHANGE_BASE_PRIORITY      0x0043
#define SYSTEM_CALL_SET_DLL_SEGMENT_WRITABLE  0x0044
#define SYSTEM_CALL_SET_DLL_SEGMENT_NOTSHARED 0x0045
#define SYSTEM_CALL_SHUTDOWN                  0x0046
#define SYSTEM_CALL_APM_BIOS                  0x0047
#define SYSTEM_CALL_MTHREAD_KILL              0x0048
#define SYSTEM_CALL_RECEIVE_PACKET            0x0049 /* not used */
#define SYSTEM_CALL_SEND_PACKET               0x0050 /* not used */
#define SYSTEM_CALL_SEMAPHORE_CREATE          0x0051
#define SYSTEM_CALL_SEMAPHORE_DOWN            0x0052
#define SYSTEM_CALL_SEMAPHORE_TRYDOWN         0x0053
#define SYSTEM_CALL_SEMAPHORE_UP              0x0054
#define SYSTEM_CALL_SEMAPHORE_DESTROY         0x0055
#define SYSTEM_CALL_SET_WATCH_POINT           0x0056
#define SYSTEM_CALL_REMOVE_WATCH_POINT        0x0057
#define SYSTEM_CALL_GET_PHYSICAL_ADDRESS      0x0058
#define SYSTEM_CALL_ALLOCATE_CONTIGUOUS       0x0059
#define SYSTEM_CALL_DEALLOCATE_CONTIGUOUS     0x0060
#define SYSTEM_CALL_CONDITION_CREATE          0x0061
#define SYSTEM_CALL_CONDITION_DESTROY         0x0062
#define SYSTEM_CALL_CONDITION_NOTIFY_ALL      0x0063
#define SYSTEM_CALL_CONDITION_WAIT            0x0064
#define SYSTEM_CALL_CONDITION_WAIT_TIMEOUT    0x0065
#define SYSTEM_CALL_NOW_IN_NANOSEC            0x0066
#define SYSTEM_CALL_MTHREAD_SELF              0x0067
#define SYSTEM_CALL_STACKTRACE_ENABLE         0x0068
#define SYSTEM_CALL_STACKTRACE_DISABLE        0x0069
#define SYSTEM_CALL_STACKTRACE_DUMP           0x0070
#define SYSTEM_CALL_MUTEX_COUNT               0x0071 /* for testablity */
#define SYSTEM_CALL_CONDITION_COUNT           0x0072 /* for testablity */
#define SYSTEM_CALL_GET_KSTAT                 0x0073
#define SYSTEM_CALL_SET_LOGGER                0x0074
enum
{
    DEBUG_BREAK_ONLY_ON_EXECUTE    = 0,
    DEBUG_BREAK_ONLY_ON_WRITE      = 1,
    DEBUG_BREAK_ONLY_ON_IO         = 2,
    DEBUG_BREAK_ONLY_ON_READ_WRITE = 3
};

#endif

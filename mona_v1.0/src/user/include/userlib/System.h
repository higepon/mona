#ifndef _MONA_USERLIB_System_
#define _MONA_USERLIB_System_

#include <types.h>

extern "C" dword syscall_get_pid();
extern "C" dword syscall_get_tid();
/*----------------------------------------------------------------------
    System
----------------------------------------------------------------------*/
class System
{
  public:
    static inline dword getProcessID()
    {
        return syscall_get_pid();
    }

    static inline dword getThreadID()
    {
        return syscall_get_tid();
    }
};

#endif

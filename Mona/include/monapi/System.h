#ifndef _MONA_USERLIB_System_
#define _MONA_USERLIB_System_

#include <sys/types.h>

extern "C" dword syscall_get_pid();
extern "C" dword syscall_get_tid();
extern "C" dword syscall_get_tick();

namespace MonAPI {

/*----------------------------------------------------------------------
    System
----------------------------------------------------------------------*/
class System
{
  public:
    inline static dword getProcessID()
    {
        return syscall_get_pid();
    }

    inline static dword getThreadID()
    {
        return syscall_get_tid();
    }

    inline static dword getTick()
    {
        return syscall_get_tick();
    }

    static PsInfo* getProcessInfo();
    static const char* getProcessPath();
    static const char* getBundlePath();
};

}

#endif

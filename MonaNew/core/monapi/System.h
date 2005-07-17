#ifndef _MONA_USERLIB_System_
#define _MONA_USERLIB_System_

#include <sys/types.h>

extern "C" dword syscall_get_pid();
extern "C" dword syscall_get_tid();
extern "C" dword syscall_get_tick();
extern "C" int syscall_kill_thread(dword tid);
extern "C" int syscall_kill();

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

    inline static int kill(dword tid)
    {
        return syscall_kill_thread(tid);
    }

    inline static int kill()
    {
        return syscall_kill();
    }

    static PsInfo* getProcessInfo();
    static dword getParentThreadID();
    static const char* getProcessPath();
    static const char* getBundlePath();
};

}

#endif

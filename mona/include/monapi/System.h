#ifndef _MONAPI_SYSTEM_
#define _MONAPI_SYSTEM_

#include <sys/types.h>
#include <monapi/Stream.h>

extern "C" uint32_t syscall_get_pid();
extern "C" uint32_t syscall_get_tid();
extern "C" uint32_t syscall_get_tick();
extern "C" int syscall_kill_thread(uint32_t tid);
extern "C" int syscall_kill();

namespace MonAPI {

/*----------------------------------------------------------------------
    System
----------------------------------------------------------------------*/
class System
{
  public:
    inline static uint32_t getProcessID()
    {
        return syscall_get_pid();
    }

    inline static uint32_t getThreadID()
    {
        return syscall_get_tid();
    }

    inline static uint32_t getTick()
    {
        return syscall_get_tick();
    }

    inline static int kill(uint32_t tid)
    {
        return syscall_kill_thread(tid);
    }

    inline static int kill()
    {
        return syscall_kill();
    }

    static PsInfo* getProcessInfo();
    static uint32_t getParentThreadID();
    static const char* getProcessPath();
    static const char* getBundlePath();
    static uint32_t getProcessStdoutID();
    static uint32_t getProcessStdinID();
    static Stream* getStdinStream();
    static Stream* getStdoutStream();
};

}

#endif

#ifndef _MONAPI_SYSTEM_
#define _MONAPI_SYSTEM_

#include <sys/types.h>
#include <monapi/Stream.h>

extern "C" uint32_t syscall_get_pid();
extern "C" uint32_t syscall_get_tid();
extern "C" uint32_t syscall_get_tick();
extern "C" int syscall_kill_thread(uint32_t tid);
extern "C" int syscall_kill(int status);

namespace MonAPI {

/*----------------------------------------------------------------------
    System
----------------------------------------------------------------------*/
class System
{
  public:
    static const char* getMoshPath()
    {
        return "/APPS/MOSH.APP/MOSH.EXE --loadpath=/LIBS/MOSH/lib";
    }

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

    inline static int kill(int status = -1)
    {
        return syscall_kill(status);
    }

    static void getProcessInfo(PsInfo* dest);
    static uint32_t getParentThreadID();
    static const char* getProcessPath();
    static const char* getBundlePath();
    static uint32_t getProcessStdoutID();
    static uint32_t getProcessStdoutID(uint32_t tid);
    static uint32_t getProcessStdinID();
    static uint32_t getProcessStdinID(uint32_t tid);
    static Stream* getStdinStream();
    static Stream* getStdoutStream();
};

}

#endif

#ifndef _MONA_USERLIB_System_
#define _MONA_USERLIB_System_

#include <types.h>

extern "C" int syscall_get_pid();
/*----------------------------------------------------------------------
    System
----------------------------------------------------------------------*/
class System {
  public:
    static inline dword getPID() {
        return syscall_get_pid();
    }
};

#endif

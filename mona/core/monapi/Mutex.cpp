/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Mutex::Mutex() : destroyed_(false)
{
    mutexId_ = syscall_mutex_create();
}

Mutex::Mutex(intptr_t mutexId) : destroyed_(false)
{
    mutexId_ = syscall_mutex_fetch(mutexId);
}

Mutex::~Mutex()
{
    if (!destroyed_) {
        destroy();
    }
}

int Mutex::lock()
{
    int result = syscall_mutex_lock(mutexId_);
    return result;
}

/*
   function: lock

   Block the callee thread until get a lock or timeout.

   Returns: 0, when the thread gets a lock. Mutex::TIMEOUT, when timeout.

*/
int Mutex::lock(intptr_t timeoutMsec)
{
    int result = syscall_mutex_lock_timeout(mutexId_, timeoutMsec);
    switch(result) {
    case M_EVENT_SLEEP:
        return TIMEOUT;
    case M_EVENT_MUTEX_UNLOCKED:
        return 0;
    default:
        ASSERT(true);
        return -1;
    }
}

int Mutex::unlock()
{
    return syscall_mutex_unlock(mutexId_);
}

int Mutex::tryLock()
{
    return syscall_mutex_trylock(mutexId_);
}

int Mutex::destroy()
{
    if (destroyed_) {
        return MONA_SUCCESS;
    }
    destroyed_ = true;
    return syscall_mutex_destroy(mutexId_);
}

}

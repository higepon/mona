/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Mutex::Mutex() : destroyed_(false)
{
    mutexId_ = syscall_mutex_create(0);
}

Mutex::Mutex(uint32_t mutexId) : destroyed_(false)
{
    syscall_mutex_create(mutexId);
    mutexId_ = mutexId;
}

Mutex::~Mutex() {

    if (!destroyed_)
    {
        destroy();
    }
}

int Mutex::lock()
{
    return syscall_mutex_lock(mutexId_);
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
    if (destroyed_) return MONA_SUCCESS;
    destroyed_ = true;
    return syscall_mutex_destroy(mutexId_);
}

}

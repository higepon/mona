/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Mutex::Mutex() : destroyed_(false)
{
    mutexId_ = syscall_mutex_create();
}

Mutex::Mutex(dword mutexId) : destroyed_(false)
{
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
    destroyed_ = true;
    return syscall_mutex_destroy(mutexId_);
}

}

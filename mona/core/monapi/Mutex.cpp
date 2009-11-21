/*!
    \file  Mutex.cpp
    \brief class kMutex

    Copyright (c) 2002-2009 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/
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

intptr_t Mutex::lock()
{
    intptr_t result = syscall_mutex_lock(mutexId_);
    return result;
}

intptr_t Mutex::lock(intptr_t timeoutMsec)
{
    return syscall_mutex_lock_timeout(mutexId_, timeoutMsec);
}

intptr_t Mutex::unlock()
{
    return syscall_mutex_unlock(mutexId_);
}

intptr_t Mutex::tryLock()
{
    return syscall_mutex_try_lock(mutexId_);
}

intptr_t Mutex::destroy()
{
    if (destroyed_) {
        return M_OK;
    }
    destroyed_ = true;
    return syscall_mutex_destroy(mutexId_);
}

}

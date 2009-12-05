#ifndef _MONAPI_MUTEX_
#define _MONAPI_MUTEX_

#include <sys/types.h>
#include <sys/error.h>
#include <monapi/syscall.h>

namespace MonAPI {

/*
    class: Mutex

*/
class Mutex
{
public:
    /*
       function: Mutex

       Creates and returns a new mutex.

       Returns:

         A mutex.
    */
    Mutex() : destroyed_(false), mutexId_(syscall_mutex_create())
    {
    }


    /*
       function: Mutex

       Fetch a mutex by mutexid, the mutex may be created by other process.
       If id is invalid, getId() returns <M_BAD_MUTEX_ID>.

       Parameters:

         id - mutexid returned by getId().

       Returns:

         A mutex.
    */
    Mutex(intptr_t id) : destroyed_(false), mutexId_(syscall_mutex_fetch(id))
    {
    }


    /*
       function: ~Mutex

       Destroy the mutex.
    */
    ~Mutex()
    {
        if (!destroyed_) {
            destroy();
        }
    }


    /*
       function: lock

       Block the callee thread until get a lock.

       Returns:
         Returns <M_OK> if the mutex is successfully locked, or <M_BAD_MUTEX_ID> if mutex is invalid.
    */
    intptr_t lock()
    {
        return syscall_mutex_lock(mutexId_);
    }


    /*
       function: lock

       Block the callee thread until get a lock or timeout.

       Parameters:

         timeoutMsec - timeout in msec.

       Returns:
         <M_OK>, when the thread gets a lock. <M_TIMED_OUT>, when timeout. <M_BAD_MUTEX_ID> if mutex is invalid.

    */
    intptr_t lock(intptr_t timeoutMsec)
    {
        return syscall_mutex_lock_timeout(mutexId_, timeoutMsec);
    }


    /*
       function: unlock

       unlock the mutex.

       Parameters:

       Returns:
         Returns <M_OK> if the mutex is successfully unlocked, or <M_BAD_MUTEX_ID> if mutex is invalid.
    */
    intptr_t unlock()
    {
        return syscall_mutex_unlock(mutexId_);
    }


    /*
       function: tryLock

       Try to lock the mutex. When the mutex is locked by other process, returns <M_BUSY>.

       Returns:
         Returns <M_OK> if the mutex is successfully locked. <M_BUSY> if the mutex is locked by other process. <M_BAD_MUTEX_ID> if mutex is invalid.

    */
    intptr_t tryLock()
    {
        return syscall_mutex_try_lock(mutexId_);
    }


    /*
       function: destroy

       Destroy the mutex. If mutex is already destroyed, do nothing.

       Returns:
         Returns <M_OK> if the mutex is successfully destoryed, or <M_BAD_MUTEX_ID> if mutexid is invalid.
    */
    intptr_t destroy();


    /*
       function: getId

       Returns:
         mutexid, which can be used with syscall_mutex functions.
    */
    inline intptr_t getId() const
    {
        return mutexId_;
    }

private:
    bool destroyed_;
    intptr_t mutexId_;
};

};

#endif

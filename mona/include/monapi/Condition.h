#ifndef _MONAPI_CONDITION_
#define _MONAPI_CONDITION_

#include <sys/types.h>
#include <sys/error.h>
#include <monapi/syscall.h>

namespace MonAPI {

/*
    class: Condition

*/
class Condition
{
public:
    /*
       function: Condition

       Creates and returns a new conditional variable.

       Returns:

         A condition variable.
    */
    Condition() : conditionId_(syscall_condition_create())
    {
    }


    /*
       function: ~Condition

       Destroy the condition.
    */
    ~Condition()
    {
        if (conditionId_ > 0) {
            syscall_condition_destroy(conditionId_);
        }
    }


    /*
       function: waitWithTimeout

       Waits on the condition until ublocked by a notify or timed out.

       Parameters:

         mutex - Mutex
         timeoutMsec - timeout in msec.

       Returns:

         Returns <M_OK> if the waiting thread is successfully unblocked. <M_TIMED_OUT>, when timeout. <M_BAD_CONDITION_ID> if condition is invalid or <M_BAD_MUTEX_ID> if mutex is invalid.

    */
    intptr_t waitWithTimeout(Mutex* mutex, intptr_t timeoutMsec)
    {
        return syscall_condition_wait_timeout(conditionId_, mutex->getId(), timeoutMsec);
    }


    /*
       function: wait

       Waits on the condition.

       Parameters:

         mutex - Mutex

       Returns:
         Returns <M_OK> if the waiting thread is successfully unblocked, <M_BAD_CONDITION_ID> if condition is invalid or <M_BAD_MUTEX_ID> if mutex is invalid.

    */
    intptr_t wait(Mutex* mutex)
    {
        return syscall_condition_wait(conditionId_, mutex->getId());
    }


    /*
       function: notifyAll

       Unblocks all the threads waiting on the condition.


       Returns:
         Returns <M_OK> if all the theads are successfully unblocked, or <M_BAD_CONDITION_ID> if condition is invalid.

    */
    intptr_t notifyAll()
    {
        return syscall_condition_notify_all(conditionId_);
    }


    /*
       function: getId

       Returns:
         conditionid, which can be used with syscall_condition functions.
    */
    intptr_t getId() const
    {
        return conditionId_;
    }

private:
    intptr_t conditionId_;
};

};

#endif

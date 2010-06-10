/*!
    \file  UserSemaphore.cpp
    \brief class UserSemaphore

    Copyright (c) 2002, 2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/27 update:$Date
*/

#include <sys/HList.h>
#include "UserSemaphore.h"
#include "io.h"
#include "syscalls.h"

/*----------------------------------------------------------------------
    UserSemaphore  *** don't allocate this object at stack! ***
----------------------------------------------------------------------*/
UserSemaphore::UserSemaphore(int sem) : refcount_(1), sem_(sem), maxSem_(sem)
{
    waitList_ = new HList<Thread*>();
}

UserSemaphore::~UserSemaphore()
{
    if (waitList_->size() != 0)
    {
        g_console->printf("UserSemaphore has waiting threads!!\n");
    }
    delete waitList_;
}

intptr_t UserSemaphore::down(Thread* thread)
{
    enter_kernel_lock_mode();
    /* lock OK */
    if (canDown())
    {
        sem_--;
    /* lock NG, so wait */
    }
    else
    {
        waitList_->add(thread);
        g_scheduler->WaitEvent(thread, MEvent::SEMAPHORE_UPPED);
        return Scheduler::YIELD;
    }
    exit_kernel_lock_mode();

    return M_OK;
}

intptr_t UserSemaphore::tryDown(Thread* thread)
{
    intptr_t result;

    enter_kernel_lock_mode();

    /* lock OK */
    if (canDown())
    {
        sem_--;
        result = M_OK;
    }
    else
    {
        result = M_BUSY;
    }

    exit_kernel_lock_mode();
    return result;
}

intptr_t UserSemaphore::up()
{
    if (sem_ == maxSem_) {
        return M_OK;
    }

    enter_kernel_lock_mode();

    if (waitList_ ->size() == 0) {
        sem_++;
    } else {
        // We first sem_++ because of up(), and next sem_-- because next thread will down().
        // So we don't modify sem_
        g_scheduler->EventComes(waitList_->removeAt(0), MEvent::SEMAPHORE_UPPED);
        return Scheduler::YIELD;
    }

    exit_kernel_lock_mode();
    return M_OK;
}

intptr_t UserSemaphore::checkSecurity(Thread* thread)
{
    return 0;
}

// void UserSemaphore::addRef()
// {
//     refcount_++;
// }

// void UserSemaphore::releaseRef()
// {
//     refcount_--;
//     if (refcount_ == 0)
//     {
//         delete this;
//     }
// }

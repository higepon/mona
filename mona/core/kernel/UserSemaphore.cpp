/*!
    \file  UserSemaphore.cpp
    \brief class UserSemaphore

    Copyright (c) 2002, 2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
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

int UserSemaphore::down(Thread* thread)
{
    enter_kernel_lock_mode();
    g_log->printf("down enter :%d:%s:%d\n", sem_, __FILE__, __LINE__);
    /* lock OK */
    if (canDown())
    {
        g_log->printf("down can down :%d:%s:%d\n", sem_, __FILE__, __LINE__);
        sem_--;
    /* lock NG, so wait */
    }
    else
    {
        g_log->printf("down wait :%d%s:%d\n", sem_, __FILE__, __LINE__);
        waitList_->add(thread);
        g_scheduler->WaitEvent(thread, MEvent::SEMAPHORE_UPPED);
        g_scheduler->SwitchToNext();

        /* not reached */
    }
    exit_kernel_lock_mode();

    return MONA_SUCCESS;
}

int UserSemaphore::tryDown(Thread* thread)
{
    int result;

    enter_kernel_lock_mode();

    /* lock OK */
    if (canDown())
    {
        sem_--;
        result = MONA_SUCCESS;
    }
    else
    {
        result = MONA_FAILURE;
    }

    exit_kernel_lock_mode();
    return result;
}

int UserSemaphore::up()
{
    g_log->printf("up enter %d:%s:%d\n", sem_, __FILE__, __LINE__);
    if (sem_ == maxSem_)
    {
        g_console->printf("maxse>>>>>>>>>>>>>>>>>.");
        return MONA_SUCCESS;
    }

    enter_kernel_lock_mode();

    if (waitList_ ->size() == 0)
    {
        sem_++;
        g_log->printf("up normal %d:%s:%d\n", sem_, __FILE__, __LINE__);
    }
    else
    {
//         sem_++;
//         sem_--;
        g_log->printf("up event notify %d:%s:%d\n", sem_, __FILE__, __LINE__);
        g_scheduler->EventComes(waitList_->removeAt(0), MEvent::SEMAPHORE_UPPED);
        g_scheduler->SwitchToNext();
        /* not reached */
    }

    exit_kernel_lock_mode();
    return MONA_SUCCESS;
}

int UserSemaphore::checkSecurity(Thread* thread)
{
    return 0;
}

void UserSemaphore::addRef()
{
    refcount_++;
}

void UserSemaphore::releaseRef()
{
    refcount_--;
    if (refcount_ == 0)
    {
        delete this;
    }
}

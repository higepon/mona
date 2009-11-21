/*!
    \file  Mutex.cpp
    \brief class kMutex

    Copyright (c) 2002, 2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/

#include <sys/HList.h>
#include <sys/error.h>
#include "Mutex.h"
#include "io.h"
#include "syscalls.h"

/*----------------------------------------------------------------------
    KMutex  *** don't allocate this object at stack! ***
----------------------------------------------------------------------*/
KMutex::KMutex() : refcount_(1), owner_(NULL)
{
    waitList_ = new HList<Thread*>();
}

KMutex::~KMutex()
{
    if (waitList_->size() != 0) {
        g_console->printf("KMutex has waiting threads!!\n");
    }
    delete waitList_;
}

intptr_t KMutex::lock(Thread* thread, int timeoutTick /* = 0 */)
{
    enter_kernel_lock_mode();
    /* lock OK */
    if (!isLocked()) {
        owner_ = thread;
    /* lock NG, so wait */
    } else if (owner_ == thread) {
        // do nothing. lock done.
    } else {
        waitList_->add(thread);

        // If lock is timed out, the scheduler will remove this thread from waitList_.
        thread->setWaitingMutex(this);
        if (0 == timeoutTick) {
            g_scheduler->WaitEvent(thread, MEvent::MUTEX_UNLOCKED);
        } else {
            g_scheduler->Sleep(thread, timeoutTick);
            // todo
            g_scheduler->WaitEvent2(thread, MEvent::SLEEP, MEvent::MUTEX_UNLOCKED);
        }
        g_scheduler->SwitchToNext();

        /* not reached */
    }
    exit_kernel_lock_mode();

    // Returns the reason for return
    return M_OK;
}

intptr_t KMutex::tryLock(Thread* thread)
{
    int result;
    enter_kernel_lock_mode();

    /* lock OK */
    if (!isLocked()) {
        owner_ = thread;
        result = M_OK;
    } else {
        result = M_BUSY;
    }

    exit_kernel_lock_mode();
    return result;
}

intptr_t KMutex::unlock()
{
    /* not locked */
    if (!isLocked()) {
        return M_OK;
    }

    enter_kernel_lock_mode();

    g_currentThread->thread->setWaitingMutex(NULL);

    if (waitList_ ->size() == 0) {
        owner_ = NULL;
    } else {
        owner_ = waitList_->removeAt(0);
        g_scheduler->EventComes(owner_, MEvent::MUTEX_UNLOCKED);
        g_scheduler->SwitchToNext();

        /* not reached */
    }
    exit_kernel_lock_mode();
    return M_OK;
}

int KMutex::checkSecurity(Thread* thread)
{
    return 0;
}

void KMutex::addRef()
{
    refcount_++;
}

void KMutex::releaseRef()
{
    refcount_--;
    if (refcount_ == 0)
    {
        delete this;
    }
}

/*!
    \file  Mutex.h
    \brief Mutex

    Copyright (c) 2004 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/
#ifndef _MONA_MUTEX_
#define _MONA_MUTEX_

#include "KObject.h"
#include "Process.h"

class Thread;
class Process;
/*----------------------------------------------------------------------
    KMutex
----------------------------------------------------------------------*/
class KMutex : public KObject {

  public:
    KMutex();
    virtual ~KMutex();

  public:
    intptr_t lock(Thread* thread, int timeout = 0);
    intptr_t tryLock(Thread* thread);
    intptr_t unlock();
    bool removeFromWaitList(Thread* thread)
    {
        return waitList_.remove(thread);
    }

    inline bool isLocked() const
    {
        return (owner_ != NULL);
    }

    inline intptr_t getType() const
    {
        return KMUTEX;
    }

  private:
    HList<Thread*> waitList_;
    Thread* owner_;
};

#endif

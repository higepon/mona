/*!
    \file  Mutex.h
    \brief Mutex

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
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
    int lock(Thread* thread, bool adaptive = false);
    int tryLock(Thread* thread);
    int unlock();

    inline bool isLocked() const {
        return (owner_ != NULL);
    }

    inline int getType() const
    {
        return KMUTEX;
    }

    int checkSecurity(Thread* thread);

  private:
    List<Thread*>* waitList_;
    Process* process_;
    Thread* owner_;
};

#endif

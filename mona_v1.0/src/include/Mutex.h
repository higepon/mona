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

#include <Process.h>

class Thread;
class Process;
/*----------------------------------------------------------------------
    KMutex
----------------------------------------------------------------------*/
class KMutex {

  public:
    KMutex(Process* process);
    virtual ~KMutex();

  public:
    int init();
    int lock(Thread* thread);
    int tryLock(Thread* thread);
    int unlock();

    inline bool isLocked() const {
        return (owner_ != NULL);
    }

    inline int getId() const {
        return id_;
    }

  private:
    static int allocateId() {
        idCount++;
        return idCount;
    }

  private:
    List<Thread*>* waitList_;
    Process* process_;
    Thread* owner_;
    int id_;
    static int idCount;

};

#endif

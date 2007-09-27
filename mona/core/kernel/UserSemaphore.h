/*!
    \file  UserSemaphore.h
    \brief UserSemaphore

    Copyright (c) 2007 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3635 $
    \date   create:2007/09/27 update:$Date$
*/
#ifndef _MONA_USER_SEMAPHORE_
#define _MONA_USER_SEMAPHORE_

#include "KObject.h"
#include "Process.h"

class Thread;
class Process;
/*----------------------------------------------------------------------
    UserSemaphore
----------------------------------------------------------------------*/
class UserSemaphore : public KObject {

  public:
    UserSemaphore(int sem);
    virtual ~UserSemaphore();

  public:
    int down(Thread* thread);
    int tryDown(Thread* thread);
    int up();
    void addRef();
    void releaseRef();

    inline bool canDown() const {
        return sem_ > 0;
    }

    inline int getType() const
    {
        return USER_SEMAPHORE;
    }

    int checkSecurity(Thread* thread);

  private:
    int refcount_;
    int sem_;
    int maxSem_;
    List<Thread*>* waitList_;
    Process* process_;
};

#endif

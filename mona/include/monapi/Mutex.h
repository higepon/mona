#ifndef _MONAPI_MUTEX_
#define _MONAPI_MUTEX_

#include <sys/types.h>
#include <sys/error.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
class Mutex
{
public:
    enum {
        TIMEOUT = -1
    };

    Mutex();
    Mutex(intptr_t mutexId);
    ~Mutex();
    int lock();
    int lock(intptr_t timeoutMsec);
    int unlock();
    int tryLock();
    int destroy();

public:
    inline intptr_t getId() const
    {
        return mutexId_;
    }

private:
    intptr_t mutexId_;
    bool destroyed_;
};

};

#endif

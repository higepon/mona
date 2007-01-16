#ifndef _MONAPI_MUTEX_
#define _MONAPI_MUTEX_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
class Mutex
{
public:
    Mutex();
    Mutex(uint32_t mutexId);
    ~Mutex();
    int lock();
    int unlock();
    int tryLock();
    int destroy();

public:
    inline int getId() const {
        return mutexId_;
    }

private:
    int mutexId_;
    bool destroyed_;
};

};

#endif

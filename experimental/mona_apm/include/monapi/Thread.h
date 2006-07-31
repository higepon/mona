#ifndef _MONA_USERLIB_THREAD_
#define _MONA_USERLIB_THREAD_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
class Mutex
{
public:
    Mutex();
    Mutex(dword mutexId);
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

}

#endif

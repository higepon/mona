#ifndef _MONA_USERLIB_THREAD_
#define _MONA_USERLIB_THREAD_

#include <types.h>

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
class Mutex {

  public:
    Mutex();
    ~Mutex();
    int init();
    int lock();
    int unlock();
    int tryLock();
    int destory();

  public:
    inline int getId() const {
        return mutexId_;
    }

  private:
    int mutexId_;
};
#endif

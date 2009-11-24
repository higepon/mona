#ifndef _MONAPI_CONDITION_
#define _MONAPI_CONDITION_

#include <sys/types.h>
#include <sys/error.h>

namespace MonAPI {

/*
    class: Condition

*/
class Condition
{
public:
    /*
       function: Condition

       Creates and returns a new conditional variable.

       Returns:

         A condition variable.
    */
    Condition();

    /*
       function: ~Condition

       Destroy the condition.
    */
    ~Condition();

    intptr_t waitWithTimeout(Mutex* mutex, intptr_t timeoutMsec);
    intptr_t wait(Mutex* mutex);
    intptr_t notifyAll();
    intptr_t getId() const
    {
        return conditionId_;
    }

private:
    intptr_t conditionId_;
};

};

#endif

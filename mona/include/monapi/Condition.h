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
};

};

#endif

#ifndef _MONA_MONES_NICFACTORY_
#define _MONA_MONES_NICFACTORY_

#include <sys/types.h>
#include "Nic.h"

namespace mones {

class NicFactory
{
public:
    static Nic* create();
};

}; // namespace mones
#endif

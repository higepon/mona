//$Id$
#pragma once
#include <sys/types.h>
#include "Nic.h"

namespace mones {

class NicFactory
{
public:
    static Nic* create();
};

}; // namespace mones

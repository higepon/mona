#include "NetServer.h"
#include <monapi.h>
#include <sys/types.h>
#pragma once

namespace mones {

class NetClient
{
    dword serverid;
    dword clientid;
public:    
    int   initalize(dword);
    int   Check();
    dword MessageLoop();
};

};

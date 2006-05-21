#include "Net.h"
#include <monapi.h>
#include <sys/types.h>
#pragma once

namespace mones {

class NetClient
{
    MessageInfo info;
    dword serverid;
    dword clientid;
    int Peek(dword);
public:
    int   initalize(dword);
    int   Open();
    int   Close();
    int   Read();
    int   Write();  
    int   Stat();
    ///////
    int   Test();
};

};

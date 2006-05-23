#include "Net.h"
#include <monapi.h>
#include <sys/types.h>
#include <monapi/messages.h>
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
    int   Config(dword, dword, word, word);
    word  GetFreePort();
    int   Open(dword, word, word, word);
    int   Close(); 
    int   Write();  
    monapi_cmemoryinfo*   Read();
    monapi_cmemoryinfo*   Stat();
    ///////
    int   Test();
};

};

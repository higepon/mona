#include "Net.h"
#include <monapi.h>
#include <sys/types.h>
#include <monapi/messages.h>
#pragma once

namespace mones {

//THIS CLASS PROVIDES APPLICATION PROGRAMING INTERFACE.
class NetClient
{
private:
    MessageInfo info;
    dword serverid;
    dword clientid;
public:
    int   initalize(dword threadid);
    int   Config(dword localip, dword gatewayip, byte subnetmask, byte timeout, word mtu);
     // localip,gatewayip must be DCBA style, 0<=subnetmask<=32,timeout(seconds) 
    word  GetFreePort();
    int   Open(dword remoteip, word localport, word remoteport, word protocol);
    // remoteip must be DCBA style, RetunValue is network descriptor.
    int   Close(int netdsc); 
    int   Write(int netdsc, byte* data,word size);  
    int   Read(int netdsc,byte* data);
    int   Stat(NetStatus* stat);
    int   Example();    //TEST FUNCTION.
};

};




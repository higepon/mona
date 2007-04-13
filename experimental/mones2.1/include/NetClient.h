//$Id$
#include <Net.h>
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
    uint32_t serverid;
    uint32_t clientid;
public:
    NetClient();
    int  Config(char* if_name, uint32_t localip, uint32_t gatewayip, uint8_t subnetmask, uint8_t timeout, uint16_t mtu);
     //  localip,gatewayip must be DCBA style, 0<=subnetmask<=32,timeout(seconds) 
    uint16_t GetFreePort();
    int  ICMPOpen(uint32_t remoteip);//ping?
    int  UDPOpen(uint32_t remopoteip, uint16_t localport, uint16_t remoteport);
    int  TCPActvOpen(uint32_t remoteip, uint16_t localport, uint16_t remoteport);
    // remoteip must be DCBA style, RetunValue is network descriptor.
    int  TCPPasvOpen( uint16_t remoteport );
    int  TCPAccept(int netdsc);
    int  Close(int netdsc); 
    int  Write(int netdsc, uint8_t* data,uint16_t size);  
    int  Read(int netdsc,uint8_t* data,bool noblock = false);
    int  Stat(NetStatus* stat);
    int  SetBlockingMode(int netdsc,int mode);
    int  Reset(uint32_t remoteip, uint16_t localport ,uint16_t remoteport);
};

};




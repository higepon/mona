//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"

namespace mones{

class Dispatch;
//Infomation structure represents Layer4
class L4Base{
public:
    uint32_t remoteip;
    uint16_t  localport;
    uint16_t  remoteport;
    uint32_t clientid;
    uint16_t  netdsc;
    uint32_t disposedtick;
    bool  disposed;
    MessageInfo msg; //too heavy? performance test is must item.
    L4Base(){msg.header=0x0; disposed=false;}
    virtual ~L4Base(){};
    virtual void CreateHeader(Ether* ,uint8_t* ,uint16_t )=0;
    virtual int  Strip(Ether*,uint8_t**)=0;
    virtual bool IsProcessed(Ether*)=0;
    virtual bool IsMyPacket(Ether*)=0;
    virtual void Close();
    virtual void Read(MessageInfo*);
    virtual void Write(MessageInfo*);
    virtual void Dump();
    virtual void SetBlockingMode(MessageInfo* msg){}
    virtual bool TimeoutCheck(uint32_t);
    void Init(uint32_t rip, uint16_t lport, uint16_t rport,uint32_t cid, uint16_t dsc){
        remoteip=rip; localport=lport; remoteport=rport; clientid=cid; netdsc=dsc;
        msg.header = MSG_NET_ACTVOPEN;
    }
protected:
    Dispatch* dispatcher;
    void CreateIPHeader(Ether*,uint16_t,uint8_t);
    uint16_t checksum(uint8_t*,uint16_t);
    bool Read_bottom_half(Ether*);
};

};


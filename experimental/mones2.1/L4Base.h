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
    dword remoteip;
    word  localport;
    word  remoteport;
    dword clientid;
    word  netdsc;
    dword disposedtick;
    bool  disposed;
    MessageInfo msg; //too heavy? performance test is must item.
    L4Base(){msg.header=0x0; disposed=false;}
    virtual ~L4Base(){};
    virtual void CreateHeader(Ether* ,byte* ,word )=0;
    virtual int  Strip(Ether*,byte**)=0;
    virtual bool IsProcessed(Ether*)=0;
    virtual bool IsMyPacket(Ether*)=0;
    virtual void Close();
    virtual void Read(MessageInfo*);
    virtual void Write(MessageInfo*);
    virtual void Read_bottom_half(Ether*);
    virtual void Write_bottom_half(Ether*){};//Nothing to do for ICMP/UDP.
    void Init(dword rip, word lport, word rport,dword cid, word dsc){
        remoteip=rip; localport=lport; remoteport=rport; clientid=cid; netdsc=dsc;
        msg.header = MSG_NET_ACTVOPEN;
    }
protected:
    Dispatch* dispatcher;
    void CreateIPHeader(Ether*,word,byte);
    word checksum(byte*,word);
};

};


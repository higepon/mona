//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"

namespace mones{

typedef struct CID{
    dword remoteip;
    word  localport;
    word  remoteport;
    word  protocol;
};

struct ConnectionInfo{
    CID Id;
    dword clientid;
    word  netdsc;
    byte  status;
    MessageInfo msg;
};

class IPStack
{
public:
    Nic* nic;
    IPStack();   
    virtual ~IPStack();
    bool initialize();  
    void dumpPacket(Ether*); 
    void ICMPreply(Ether*);
    word checksum(byte*,word);
    void FillIPHeader(Ether*,word,byte);
    void FillICMPHeader(ICMP*,ICMP*);
    int Send(byte* ,int, CID*);
    CID* CheckDst(int);
    byte* Recv(int);
    void PeriodicUpdate();
};

};


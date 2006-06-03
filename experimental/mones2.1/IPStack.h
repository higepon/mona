//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"

namespace mones{

struct CID{
    dword remoteip;
    word  localport;
    word  remoteport;
    byte  protocol;
    bool equal(const CID& id){
        if( remoteip   == id.remoteip   &&
            localport  == id.localport  &&
            remoteport == id.remoteport &&
            protocol   == id.protocol 
         ){
            return true;
        }else return false;
    }    
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
private:
    void ICMPreply(Ether*);
    word checksum(byte*,word);   
    void FillIPHeader(Ether*,word,byte);
    void FillICMPHeader(ICMP*,ICMP*);
    bool UDPWellKnownSVCreply(Ether*);
    bool HandshakePASV(Ether*);
    void HandshakeACTV(Ehter*);
    Nic* nic;
public:
    IPStack();
    virtual ~IPStack();
    bool initialize();
    int  Send(byte* ,int, CID*);
    bool GetDestination(int,CID*);
    int  Recv(byte**,int);
    int  interrupt(){ return nic->interrupt();}
    void readStatus(NetStatus* stat){ nic->getStatus(stat); }
    void Dispose(int);
    void PeriodicUpdate();
};

};


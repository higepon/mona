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
    word  protocol;
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
public:
    Nic* nic;
    IPStack();   
    virtual ~IPStack();
    bool initialize();  
    void ICMPreply(Ether*);
    word checksum(byte*,word);
    void FillIPHeader(Ether*,word,byte);
    void FillICMPHeader(ICMP*,ICMP*);
    int Send(byte* ,int, CID*);
    int CheckDst(int,CID*);
    int Recv(byte**,int);
    void PeriodicUpdate();
};

};


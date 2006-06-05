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
            protocol   == id.protocol )
         {
            return true;
         }else{
             return false;
         }
    }    
};

struct ConnectionInfo{
    enum TCP_STAT{
        CLOSED     =0,
        LISTENING  =1,
        SYN_SENT   =2,
        SYN_RCVD   =3,
        ESTABLISHED=4,
        FIN_WAIT1  =5,
        FIN_WAIT2  =6,
        CLOSE_WAIT =7,
        LAST_ACK   =8,
        TIME_WAIT  =9
    };
    CID Id;
    struct TCPInfo{
        dword seqnum;
        dword acknum;
        byte  status;
        byte  flag;
    };
    struct ICMPInfo{
        word type;
        word seqnum;
        word idnum;
    };
    union PKTINFO{ 
        ICMPInfo icmpinfo;
        TCPInfo  tcpinfo;
    } PktInfo;
    dword clientid;
    word  netdsc;
    //too heavy? performance test is must item.
    MessageInfo msg;
};

class IPStack
{
private:
    word checksum(byte*,word);   
    void CreateIPHeader(Ether*,word,byte);
    void CreateTCPHeader(Ether*,byte,byte);
    bool UDPWellKnownSVCreply(Ether*);
    bool HandShakePASV(Ether*);
    bool HandShakeACTV(Ether*);
    Nic* nic;
public:
    IPStack();
    virtual ~IPStack();
    bool initialize();
    int  Send(byte* ,int, ConnectionInfo* );//CID*);
    bool GetDestination(int,ConnectionInfo*);
    int  Recv(byte**,int);
    int  interrupt(){ return nic->interrupt();}
    void readStatus(NetStatus* stat){ nic->getStatus(stat); }
    void Dispose(int);
    void PeriodicUpdate();
};

};


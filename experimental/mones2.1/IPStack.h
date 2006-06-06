//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"

namespace mones{

//Infomation structure represents a connection.
class ConnectionInfo{
public:
    dword remoteip;
    word  localport;
    word  remoteport;
    byte  protocol;
    bool ident(const ConnectionInfo* cinfo){
        if( remoteip   == cinfo->remoteip   &&
            localport  == cinfo->localport  &&
            remoteport == cinfo->remoteport &&
            protocol   == cinfo->protocol )
         {
            return true;
         }else{
             return false;
         }
    }
    dword clientid;
    word  netdsc;
    MessageInfo msg; //too heavy? performance test is must item.
    virtual void CreateHeader(Ether* frame,word size){};
private:
    void CreateIPHeader(Ether*,word);
};

class ICMPCoInfo : public ConnectionInfo
{
public:
    word type;
    word seqnum;
    word idnum;
    void CreateHeader(Ether*,word){};
};

class UDPCoInfo : public ConnectionInfo
{ 
public:
    void CreateHeader(Ether* frame ,word size){};
};

class TCPCoInfo : public ConnectionInfo
{
public:
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
    dword seqnum;
    dword acknum;
    byte  status;
    byte  flag;
    void CreateHeader(Ether*,word){};
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
    int  Send(byte* ,int, ConnectionInfo* );
    bool GetDestination(int,ConnectionInfo*);
    int  Recv(byte**,int);
    int  interrupt(){ return nic->interrupt();}
    void readStatus(NetStatus* stat){ nic->getStatus(stat); }
    void PeriodicUpdate();
    void Dispose(int n){ nic->Delete(n); }
};

};


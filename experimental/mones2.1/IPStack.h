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
    virtual void CreateHeader(Ether* ,byte* ,word ){}
    virtual int Strip(Ether*,byte**){return 0;}
    void CreateIPHeader(Ether*,word);
    word checksum(byte*,word); 
private:   

};

class ICMPCoInfo : public ConnectionInfo
{
public:
    word type;
    word seqnum;
    word idnum;
    void CreateHeader(Ether* ,byte*,word );
    int Strip(Ether*,byte**);
};

class UDPCoInfo : public ConnectionInfo
{ 
public:
    void CreateHeader(Ether* ,byte* ,word );  
    int Strip(Ether*, byte**);
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
    void CreateHeader(Ether*,byte* ,word);  
    int Strip(Ether*, byte**);
    bool HandShakePASV(Ether*);
    bool HandShakeACTV(Ether*);
};

class IPStack
{
private:
    Nic* nic;    
    bool UDPWellKnownSVCreply(Ether*);
public:
    IPStack();
    virtual ~IPStack();
    bool initialize();
    int  Send(byte* ,int, ConnectionInfo* );
    bool GetDestination(int,ConnectionInfo*);
    int  Strip(Ether*,byte**);
    int  interrupt(){ return nic->interrupt();}
    void readStatus(NetStatus* stat){ nic->getStatus(stat); }
    void PeriodicUpdate();   
    void read_bottom_half(int,ConnectionInfo*);
    void Dispose(int n){ nic->Delete(n); }   
    HList<ConnectionInfo*> cinfolist;
};

};


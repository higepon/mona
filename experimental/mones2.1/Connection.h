//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include "Nic.h"

namespace mones{

class Dispatch;
//Infomation structure represents a connection.
class ConnectionInfo{
public:
    dword remoteip;
    word  localport;
    word  remoteport;
    dword clientid;
    word  netdsc;

    MessageInfo msg; //too heavy? performance test is must item.
    ConnectionInfo(){msg.header=0x0;}
    virtual ~ConnectionInfo(){};
    virtual void CreateHeader(Ether* ,byte* ,word )=0;
    virtual int  Strip(Ether*,byte**)=0;
    virtual bool IsMyPacket(Ether*)=0;
    virtual void Close(){};
    virtual word getType()=0; //RTTI is Disabled.
    void Init(dword rip, word lport, word rport,dword cid, word dsc){
        remoteip=rip; localport=lport; remoteport=rport; clientid=cid; netdsc=dsc;
    }
protected:
    Dispatch* dispatcher;
    void CreateIPHeader(Ether*,word,byte);
    word checksum(byte*,word);

private:
    virtual bool WellKnownSVCreply(Ether*){return false;}
};

class ICMPCoInfo : public ConnectionInfo
{
public:
    word type;
    word seqnum;
    word idnum;
    ICMPCoInfo(Dispatch* p){dispatcher=p;}
    void CreateHeader(Ether* ,byte*,word );
    int Strip(Ether*,byte**);
    bool IsMyPacket(Ether*);    
    word getType(){return TYPEICMP;}
private:
    bool WellKnownSVCreply(Ether*);
};

class UDPCoInfo : public ConnectionInfo
{ 
public:
    UDPCoInfo(Dispatch* p){dispatcher=p;}
    void CreateHeader(Ether* ,byte* ,word );  
    int Strip(Ether*, byte**);
    bool IsMyPacket(Ether*);  
    word getType(){return TYPEUDP;}
private:
    bool WellKnownSVCreply(Ether*);
};

class TCPCoInfo : public ConnectionInfo
{
public:
    TCPCoInfo(Dispatch* p){ dispatcher=p; status=CLOSED; isPasv=true;}
    void CreateHeader(Ether*,byte* ,word);  
    int  Strip(Ether*, byte**);
    bool IsMyPacket(Ether*);
    void Close();    
    word getType(){return TYPETCP;}
    bool isPasv;    
    bool TransStateByPKT(Ether*);
    bool TransStateByMSG(dword);
private:
    dword seqnum;
    dword acknum;
    byte  status;
    byte  flags;
    word  window;
    enum TCP_STAT{
        CLOSED=1,
        LISTENING,
        SYN_SENT,
        SYN_RCVD,
        ESTAB,
        FIN_WAIT1,
        FIN_WAIT2,
        CLOSE_WAIT,
        LAST_ACK,
        TIME_WAIT
    };
    enum CTRLFLAGS{
        FIN=0x01,
        SYN=0x02,
        RST=0x04,
        PSH=0x08,
        ACK=0x10,    
        URG=0x20,
        ECN=0x40,
        RED=0x80
    };
    bool WellKnownSVCreply(Ether*);

};

};


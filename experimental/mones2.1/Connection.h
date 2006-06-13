//$Id: Dispatch.h 3263 2006-06-09 18:05:20Z eds1275 $
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
private:
    bool WellKnownSVCreply(Ether*);
};

class TCPCoInfo : public ConnectionInfo
{
public:
    TCPCoInfo(Dispatch* p){ dispatcher=p; status=LISTENING;}
    void CreateHeader(Ether*,byte* ,word);  
    int  Strip(Ether*, byte**);
    bool IsMyPacket(Ether*);
    void Close();
private:
    dword seqnum;
    dword acknum;
    byte  status;
    byte  flags;
    word  window;
    enum TCP_STAT{
        LISTENING=1,
        SYN_SENT,
        SYN_RCVD,
        ESTABLISHED,
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
        RED=0x80,
    };
    bool WellKnownSVCreply(Ether*);
    bool HandShakePASV(Ether*);
    bool HandShakeACTV(Ether*);
};

};


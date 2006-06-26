//$Id: Connection.h 3287 2006-06-18 07:17:51Z eds1275 $
#pragma once
#include "L4Base.h"

namespace mones{

class TCPCoInfo : public L4Base
{
public:
    void Dump();
    TCPCoInfo(Dispatch*);
    void CreateHeader(Ether*,byte* ,word);  
    int  Strip(Ether*, byte**);
    bool IsMyPacket(Ether*);
    bool IsProcessed(Ether*);
    void Close();
    void Write(MessageInfo*);
    bool isPasv;    
    bool TransStateByPKT(Ether*);
    bool TransStateByMSG(dword);
    void SendACK(Ether*);
    void ReplyUnReach(Ether*);
    void Accept(MessageInfo*);
    enum CTRLFLAGS{
        NORM=0x0,
        FIN=0x01,
        SYN=0x02,
        RST=0x04,
        PSH=0x08,
        ACK=0x10,    
        URG=0x20,
        ECN=0x40,
        RED=0x80
    };
private:
    int serialno;
    dword seqnum;
    dword acknum;
    byte  status;
    byte  flags;
    word  window;
    enum TCP_STAT{
        CLOSED=1,
        LISTEN,
        SYN_SENT,
        SYN_RCVD,
        ESTAB,
        FIN_WAIT1,
        FIN_WAIT2,
        CLOSE_WAIT,
        LAST_ACK,
        TIME_WAIT
    };
    int Duplicate();
    bool WellKnownSVCreply(Ether*);
    void Write_bottom_half(Ether*);
    void Read_bottom_half(Ether*);
};

};


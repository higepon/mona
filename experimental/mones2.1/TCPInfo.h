//$Id: Connection.h 3287 2006-06-18 07:17:51Z eds1275 $
#pragma once
#include "L4Base.h"

namespace mones{

class TCPCoInfo : public L4Base
{
public:
    void Dump();
    TCPCoInfo(Dispatch*);
    bool IsExpected(Ether*);
    bool IsMyPacket(Ether*);
    bool IsProcessed(Ether*);
    void Close();  
    void Read(MessageInfo*);
    void Write(MessageInfo*);
    bool TransStateByPKT(Ether*);
    bool PasvOpen();
    bool ActvOpen();
    void SendACK(Ether*);
    void ReplyUnReach(Ether*);
    void Accept(MessageInfo* m){
        memcpy(&msg,(uint8_t*)m,sizeof(MessageInfo));
    }
    void Reset(uint32_t, uint16_t, uint16_t);
    bool TimeoutCheck(uint32_t);
    void SetBlockingMode(MessageInfo*);//(;_;)
private:    

    uint32_t write_timeout;
    uint32_t read_timeout;
    uint32_t blockingmode;
    bool nomoredata;
    uint32_t seqnum;
    uint32_t acknum;
    uint8_t  status;
    uint8_t  flags;
    uint16_t  window;
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
        TIME_WAIT,
        CLOSING
    };  
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
    bool need_retry;
    int  Duplicate();
    void Write_bottom_half(Ether*);
    bool Write_retry();
    bool Read_bottom_half(Ether*);    
    void CreateHeader(Ether*,uint8_t* ,uint16_t);  
    int  Strip(Ether*, uint8_t**);
};

};


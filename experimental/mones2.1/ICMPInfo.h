//$Id: Connection.h 3287 2006-06-18 07:17:51Z eds1275 $
#pragma once
#include "L4Base.h"

namespace mones{

class ICMPCoInfo : public L4Base
{
public:
    uint16_t type;
    uint16_t seqnum;
    uint16_t idnum;
    ICMPCoInfo(Dispatch* p){dispatcher=p;idnum=dispatcher->getPacketID();seqnum=idnum;disposed=true;
        disposedtick=syscall_get_tick()+500;
    }
    ~ICMPCoInfo(){};
    void CreateHeader(Ether* ,uint8_t*,uint16_t );
    int Strip(Ether*,uint8_t**);
    bool IsMyPacket(Ether*);    
    bool IsProcessed(Ether*);
    //uint16_t getType(){return TYPEICMP;}  
    bool Reply(Ether*);
    void Dump();
private:

};


};


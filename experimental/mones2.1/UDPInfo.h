//$Id: Connection.h 3287 2006-06-18 07:17:51Z eds1275 $
#pragma once
#include "L4Base.h"

namespace mones{

class UDPCoInfo : public L4Base
{ 
public:
    UDPCoInfo(Dispatch* p){dispatcher=p;}
    void CreateHeader(Ether* ,uint8_t* ,uint16_t );  
    int Strip(Ether*, uint8_t**);
    bool IsMyPacket(Ether*);
    bool IsProcessed(Ether*);
    bool Reply(Ether*);
};

};


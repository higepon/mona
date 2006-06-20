//$Id: Connection.h 3287 2006-06-18 07:17:51Z eds1275 $
#pragma once
#include "L4Base.h"

namespace mones{

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

};


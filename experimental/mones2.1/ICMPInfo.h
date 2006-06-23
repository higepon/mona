//$Id: Connection.h 3287 2006-06-18 07:17:51Z eds1275 $
#pragma once
#include "L4Base.h"

namespace mones{

class ICMPCoInfo : public L4Base
{
public:
    word type;
    word seqnum;
    word idnum;
    ICMPCoInfo(Dispatch* p){dispatcher=p;}
    void CreateHeader(Ether* ,byte*,word );
    int Strip(Ether*,byte**);
    bool IsMyPacket(Ether*);    
    bool IsProcessed(Ether*);
    word getType(){return TYPEICMP;}  
    bool Reply(Ether*);
private:

};


};


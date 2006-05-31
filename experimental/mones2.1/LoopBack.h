//$Id$ 
#pragma once
#include <sys/types.h>
#include <monapi.h>
#include "Nic.h"

namespace mones{ 

class LoopBack : public Nic 
{
public:    
    int   init(){return 0;};
    void  SendFrm(Ether*);
    int   interrupt();
    LoopBack();
    ~LoopBack();
    void  getMacAddress(byte* dest){memcpy(dest, mac_addr, 6); }
private:
    byte mac_addr[6];
};

};//mones::

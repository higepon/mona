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
    void  getMacAddress(uint8_t* dest){memcpy(dest, mac_addr, 6); }
private:
    uint8_t mac_addr[6];
};

};//mones::

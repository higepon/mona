//$Id$
#pragma once

#include "Nic.h"

namespace mones{


class IPStack
{
public:   
    Nic* nic;
    IPStack();   
    virtual ~IPStack();
    bool initialize();
    void ICMPreply(IP*);
    bool Match(byte* ,IP*);
    word checksum(byte*,word);
    void FillIPHeader(IP*);
    void dumpPacket(IP*); 
    void FillICMPHeader(ICMP*,ICMP*);
};

};


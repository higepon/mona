#ifndef __IPADDRESS_H__
#define __IPADDRESS_H__

extern "C" {
#include <uip.h>
}

namespace mones {

class IPAddress
{
public:
    IPAddress(byte a, byte b, byte c, byte d) {uip_ipaddr(address_, a, b, c, d);}
    virtual ~IPAddress() {}
    word* GetAddress() {return address_;}

protected:
    word address_[2];
};

}; // namespace mones

#endif // __IPADDRESS_H__


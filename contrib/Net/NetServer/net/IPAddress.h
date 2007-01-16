#ifndef __IPADDRESS_H__
#define __IPADDRESS_H__

extern "C" {
#include <uip.h>
}

namespace mones {

class IPAddress
{
public:
    IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {uip_ipaddr(address_, a, b, c, d);}
    virtual ~IPAddress() {}
    uint16_t* GetAddress() {return address_;}

protected:
    uint16_t address_[2];
};

}; // namespace mones

#endif // __IPADDRESS_H__


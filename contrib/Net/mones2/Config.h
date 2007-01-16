#ifndef _MONA_MONES_CONFIG_
#define _MONA_MONES_CONFIG_

#include <sys/types.h>
#include <monapi.h>
#include "Util.h"

namespace mones {

class Config
{
public:
    Config(uint32_t ipAddress, uint32_t subnetAddress, uint32_t defaultGateway)
    {
        this->ipAddress      = ipAddress;
        this->subnetAddress  = subnetAddress;
        this->defaultGateway = defaultGateway;
    }
    virtual ~Config() {}

    uint32_t getIpAddress() const {return this->ipAddress;}
    uint32_t getSubnetAddress() const {return this->subnetAddress;}
    uint32_t getDefaultGateway() const {return this->defaultGateway;}

private:
    uint32_t ipAddress;
    uint32_t subnetAddress;
    uint32_t defaultGateway;
};

}; // namespace mones
#endif

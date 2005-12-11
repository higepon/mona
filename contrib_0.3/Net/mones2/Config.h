#ifndef _MONA_MONES_CONFIG_
#define _MONA_MONES_CONFIG_

#include <sys/types.h>
#include <monapi.h>
#include "Util.h"

namespace mones {

class Config
{
public:
    Config(dword ipAddress, dword subnetAddress, dword defaultGateway)
    {
        this->ipAddress      = ipAddress;
        this->subnetAddress  = subnetAddress;
        this->defaultGateway = defaultGateway;
    }
    virtual ~Config() {}

    dword getIpAddress() const {return this->ipAddress;}
    dword getSubnetAddress() const {return this->subnetAddress;}
    dword getDefaultGateway() const {return this->defaultGateway;}

private:
    dword ipAddress;
    dword subnetAddress;
    dword defaultGateway;
};

}; // namespace mones
#endif

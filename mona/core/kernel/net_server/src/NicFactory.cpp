#include "NicFactory.h"
#include "NE2000.h"

using namespace mones;

Nic* NicFactory::create()
{
    Nic* nic;

    nic = new NE2000();
    nic->setIRQ(11);
    nic->setIOBase(0xC100);
    if (nic->init())
    {
        return NULL;
    }
    return nic;
}

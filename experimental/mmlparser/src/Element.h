#ifndef __ELEMENT_H__
#define __ELEMENT_H__

namespace MML {

#include "types.h"

class Element
{
public:
    Element(bool rest = false);
    virtual ~Element();

    bool isRest() const { return rest; }

public:
    dword hz;
    dword ms;
    bool rest;
};

}; // namespace MML

#endif // __ELEMENT_H__

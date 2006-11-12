#ifndef __ELEMENT_H__
#define __ELEMENT_H__

namespace MML {

#ifdef MONA
#include <sys/types.h>
#else
#include <types.h>
#endif

class Element
{
public:
    Element(bool rest = false);
    virtual ~Element();

    bool isRest() const { return rest; }

public:
    dword hz;
    dword ms;
    word volume;
    bool rest;
};

}; // namespace MML

#endif // __ELEMENT_H__

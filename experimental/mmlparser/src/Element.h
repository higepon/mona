#ifndef __ELEMENT_H__
#define __ELEMENT_H__

namespace MonAPI {
namespace audio {
namespace mml {

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

}; // namespace mml
};
};

#endif // __ELEMENT_H__

#ifndef _MONAPI_AUDIO_MML_ELEMENT_
#define _MONAPI_AUDIO_MML_ELEMENT_

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
    uint32_t hz;
    uint32_t ms;
    uint16_t volume;
    bool rest;
};

}; // namespace mml
};
};

#endif // __MONAPI_AUDIO_MML_ELEMENT_H__

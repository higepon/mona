#ifndef _MONAPI_AUDIO_MML_CONFIG_
#define _MONAPI_AUDIO_MML_CONFIG_

#ifdef MONA
#include <sys/types.h>
#else
#include <types.h>
#endif

namespace MonAPI {
namespace audio {
namespace mml {

class Config
{
public:
    Config();
    virtual ~Config();

public:
    uint32_t tempo;
    uint16_t volume;
    uint8_t octave;
    uint8_t q;
    uint8_t l;

public:
    enum
    {
        DEFAULT_VOLUME = 10,
        DEFAULT_TEMPO = 120,
        DEFAULT_OCTAVE = 4,
        DEFAULT_L = 4,
    };

};

}; // namespace mml
};
};

#endif // __MONAPI_AUDIO_MML_CONFIG_H__

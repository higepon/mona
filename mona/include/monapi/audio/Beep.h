#ifndef _MONAPI_AUDIO_BEEP_
#define _MONAPI_AUDIO_BEEP_

#include "mml/Parser.h"

namespace MonAPI {
namespace audio {

class Beep
{
public:
    Beep();
    virtual ~Beep();

    void beep(uint32_t hz, uint32_t ms);
    void rest(uint32_t ms);
    void play(const char* mml);
    std::string title(const char* mml);

protected:
    mml::Parser parser;
};

}; // namespace audio
}; // namespace MonAPI

#endif // _MONAPI_AUDIO_BEEP_

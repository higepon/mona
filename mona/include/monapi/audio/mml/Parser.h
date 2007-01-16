#ifndef _MONAPI_AUDIO_MML_PARSER_
#define _MONAPI_AUDIO_MML_PARSER_

#ifdef MONA
#include <monalibc.h>
#include <sys/types.h>
#else
#include <stdio.h>
#include "types.h"
#endif

#include <string>
#include <vector>
#include "Config.h"
#include "Element.h"

namespace MonAPI {
namespace audio {
namespace mml {

typedef std::vector<Element*> Channel;
typedef std::vector<Channel*> Channels;

class Parser
{
public:
    Parser();
    virtual ~Parser();

    Channels* parse(const std::string& text);
    void destroyChannels(Channels* channels);
    std::string title() {return title_;};

protected:
    int parseInternal();
    char getChar();
    void unGetChar();
    bool getNumber(double* number);
    uint32_t calcMilliSecond(double value);
    void warn(const char* msg);
    Channel* getChannel() { return channel_; }
    Channel* channel_;
    Channels* channels_;
    Config* config_;
    std::string text_;
    std::string title_;
    int postion_;
    static const uint32_t pitchTable[];
    static const uint8_t pitchIndex[];
};

};
};
};
#endif // __MONAPI_AUDIO_MML_PARSER_H__

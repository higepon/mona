#include <monapi.h>
#include <monapi/io.h>
#include <monapi/audio/Beep.h>

using namespace MonAPI::audio;

Beep::Beep()
{
}

Beep::~Beep()
{
}

void Beep::beep(uint32_t hz, uint32_t ms)
{
    uint16_t value = (uint16_t)(1193180 / hz);
    outp8(0x61, inp8(0x61) & 0x0d);
    outp8(0x43, 0xb6);
    outp8(0x42, value & 0xff);
    outp8(0x42, value >> 8);
    outp8(0x61, (inp8(0x61) | 0x03) & 0x0f);
    sleep(ms);
    // do not off beep!
}

void Beep::rest(uint32_t ms)
{
    // off beep
    outp8(0x61, inp8(0x61) & 0x0d);
    sleep(ms);
}

void Beep::play(const char* mml)
{
    mml::Channels* channels = parser.parse(mml);

    std::vector<mml::Channel::iterator> its;
    uint8_t channelNum = channels->size();
    const uint8_t ms = 10;
    for (int i = 0; i < channelNum; i++)
    {
        its.push_back(channels->at(i)->begin());
    }

    for (;;)
    {
        for (int i = 0; i < channelNum; i++)
        {
            mml::Channel::iterator it = its[i];
            mml::Element* e = *it;
            if ((int)e->ms <= 0)
            {
                delete (e);
                ++it;
                if (it == channels->at(i)->end()) return;
                its[i] = it;
            }
            if (e->isRest()) {
                rest(ms);
            } else {
                beep(e->hz, ms);
            }
            e->ms -= ms * channelNum;
        }
    }
    parser.destroyChannels(channels);
}

std::string Beep::title(const char* mml)
{
    mml::Channels* channels = parser.parse(mml);
    std::string t = parser.title();
    parser.destroyChannels(channels);
    return t;
}

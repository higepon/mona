#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef MONA
#include <sys/types.h>
#else
#include <types.h>
#endif

namespace MML {

class Config
{
public:
    Config();
    virtual ~Config();

public:
    dword tempo;
    word volume;
    byte octave;
    byte q;
    byte l;

public:
    enum
    {
        DEFAULT_VOLUME = 10,
        DEFAULT_TEMPO = 120,
        DEFAULT_OCTAVE = 4,
        DEFAULT_L = 4,
    };

};

}; // namespace MML

#endif // __CONFIG_H__

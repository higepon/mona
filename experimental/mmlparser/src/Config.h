#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <types.h>

namespace MML {

class Config
{
public:
    Config();
    virtual ~Config();

public:
    dword tempo;
    byte octave;
    byte q;
    byte l;

protected:
    enum
    {
        DEFAULT_TEMPO = 120,
        DEFAULT_OCTAVE = 4,
        DEFAULT_L = 4,
    };

};

}; // namespace MML

#endif // __CONFIG_H__

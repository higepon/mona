#ifndef __PARSER_H__
#define __PARSER_H__

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

namespace MML
{

typedef std::vector<Element*> Channel;
typedef std::vector<Channel*> Channels;

class Parser
{
public:
    Parser();
    virtual ~Parser();

    Channels* parse(const std::string& text);

protected:
    int parseInternal();
    char getChar();
    void unGetChar();
    bool getNumber(double* number);
    dword calcMilliSecond(double value);
    void warn(const char* msg);
    Channel* getChannel() { return channel_; }
    Channel* channel_;
    Channels* channels_;
    Config* config_;
    std::string text_;
    int postion_;
    static const dword pitchTable[];
    static const byte pitchIndex[];
};

};
#endif // __PARSER_H__

#include <monapi/audio/mml/Config.h>

using namespace MonAPI::audio::mml;

Config::Config() : tempo(DEFAULT_TEMPO), volume(DEFAULT_VOLUME), octave(DEFAULT_OCTAVE), l(DEFAULT_L)
{
}

Config::~Config()
{
}

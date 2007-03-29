#include "StringReader.h"

using namespace util;
using namespace monash;

StringReader::StringReader(const String& text) : text_(text), position_(0)
{
}

StringReader::~StringReader()
{
}

char StringReader::readChar()
{
    if (position_ >= text_.size()) return EOF;
    char c = text_[position_];
    position_++;
    return c;
}

void StringReader::unReadChar(char c)
{
    position_--;
    text_.set(position_, c);
}

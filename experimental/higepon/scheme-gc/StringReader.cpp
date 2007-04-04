#include "StringReader.h"

using namespace util;
using namespace monash;

StringReader::StringReader(const String& text) : text_(text), position_(0), lineno_(1)
{
}

StringReader::~StringReader()
{
}

char StringReader::readChar()
{
    if (position_ >= (int)text_.size()) return EOF;
    char c = text_[position_];
    if (c == '\n') lineno_++;
    position_++;
    return c;
}

void StringReader::unReadChar(char c)
{
    if (c == EOF) return;
    if (c == '\n') lineno_--;
    position_--;
    text_.set(position_, c);
}

::util::String StringReader::getFileName()
{
    return "user input";
}

int StringReader::getLineNo()
{
    return lineno_;
}


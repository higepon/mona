/*!
    \file   StringReader.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
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
    logprintf("text_ %x text.size=%d %s %s:%d\n", &text_, text_.size(), __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (position_ >= (int)text_.size())
    {
        logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return EOF;
    }
    logprintf("position_ = %d %s %s:%d\n", position_, __func__, __FILE__, __LINE__);fflush(stdout);// debug
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


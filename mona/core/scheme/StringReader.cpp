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
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (position_ >= (int)text_.size()) return EOF;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    char c = text_[position_];
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (c == '\n') lineno_++;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    position_++;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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


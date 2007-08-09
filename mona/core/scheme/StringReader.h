/*!
    \file  StringReader.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __STRING_READER_H__
#define __STRING_READER_H__

#include "Reader.h"

namespace monash {

class StringReader : public Reader
{
public:
    StringReader(const ::util::String& text);
    virtual ~StringReader();
    virtual char readChar();
    virtual void unReadChar(char c);
    virtual ::util::String getFileName();
    virtual int getLineNo();

protected:
    ::util::String text_;
    int position_;
    int lineno_;
};

}; // namespace monash
#endif // __STRING_READER_H__

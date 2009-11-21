/*!
    \file  Reader.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __READER_H__
#define __READER_H__

#include "util/String.h"

namespace monash {

class Reader
{
public:
    Reader() {}
    virtual ~Reader() {}
    virtual char readChar() = 0;
    virtual void unReadChar(char c) = 0;
    virtual ::util::String getFileName() = 0;
    virtual int getLineNo() = 0;
};

}; // namespace monash
#endif // __READER_H__

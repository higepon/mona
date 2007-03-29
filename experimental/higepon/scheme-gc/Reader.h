#ifndef __READER_H__
#define __READER_H__

#include "util/String.h"

namespace monash {

class Reader
{
public:
    virtual char readChar() = 0;
    virtual void unReadChar(char c) = 0;
};

}; // namespace monash
#endif // __READER_H__

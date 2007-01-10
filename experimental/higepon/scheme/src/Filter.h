#ifndef __FILTER_H__
#define __FILTER_H__

#include "Node.h"

namespace monash {

class Filter
{
public:
    Filter() {}
    virtual ~Filter() {}
    virtual int filter(SExp* from) = 0;
    enum
    {
        SYNTAX_ERROR,
        SUCCESS,
    };
};

};
#endif // __FILTER_H__

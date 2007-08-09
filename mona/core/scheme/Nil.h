/*!
    \file  Nil.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __NIL__
#define __NIL__

#include "Object.h"
//#include "scheme.h"

namespace monash {

class Nil : public Object
{
public:
    Nil(uint32_t lineno = 0);
    virtual ~Nil();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "()"; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

protected:
    uint32_t lineno_;
};

}; // namespace monash

#endif // __NIL__

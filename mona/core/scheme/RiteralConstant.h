/*!
    \file  RiteralConstant.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __RITERAL_CONSTANT__
#define __RITERAL_CONSTANT__

#include "Object.h"
#include "scheme.h"

namespace monash {

class RiteralConstant : public Object
{
public:
    RiteralConstant(const ::util::String& value, uint32_t lineno = 0);
    virtual ~RiteralConstant();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "riteral constant"; }
    virtual ::util::String text() const { return value_;}
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

protected:
    ::util::String value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __RITERAL_CONSTANT__

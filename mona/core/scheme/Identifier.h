/*!
    \file  Identifier.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __IDENTIFIER_H__
#define __IDENTIFIER_H__

#include "Object.h"
#include "scheme.h"

namespace monash {

class Identifier : public Object
{
public:
    Identifier(const ::util::String& value, uint32_t lineno = 0);
    virtual ~Identifier();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const { return Object::IDENTIFIER; }
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "identifier"; }
    virtual ::util::String text() const { return value_;}
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);

protected:
    ::util::String value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __IDENTIFIER_H__

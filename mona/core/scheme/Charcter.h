/*!
    \file  Charcter.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __CHARCTER_H__
#define __CHARCTER_H__

#include "Object.h"
#include "Number.h"
namespace monash {

class Charcter : public Object
{
public:
    Charcter(const ::util::String& value, uint32_t lineno = 0);
    Charcter(char value, uint32_t lineno = 0);
    virtual ~Charcter();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const { return Object::CHARCTER; }
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "char"; }
    virtual char value() { return value_; }
    virtual ::util::String stringValue();
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool gt(Object* o);
    virtual bool lt(Object* o);
    virtual bool ge(Object* o);
    virtual bool le(Object* o);
    virtual Number* toNumber();
    static Charcter* fromNumber(Number* n);

protected:
    char value_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __CHARCTER_H__

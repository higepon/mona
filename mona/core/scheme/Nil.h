/*!
    \file  Nil.h
    \brief

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/02 update:$Date$
*/

#ifndef __NIL__
#define __NIL__

#include "Object.h"

namespace monash {

class Nil : public Object
{
private:
    Nil() {}
    virtual ~Nil() {}

public:
    static Nil* createInstance()
    {
        static Nil* n = NULL;
        if (NULL == n)
        {
            n = new Nil();
        }
        return n;
    }

public:
    virtual ::util::String toString()
    {
        return "()";
    }
    virtual ::util::String toStringValue()
    {
        return "()";
    }
    virtual int type() const
    {
        return Object::NIL;
    }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual uint32_t lineno() const
    {
        return 0;
    }
    virtual ::util::String typeString() const
    {
        return "()";
    }
    virtual bool eqv(Object* o)
    {
        return o->isNil();
    }
    virtual bool eq(Object* o)
    {
        return o->isNil();
    }
};

}; // namespace monash

#endif // __NIL__

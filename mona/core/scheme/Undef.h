/*!
    \file  Undef.h
    \brief

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/02 update:$Date$
*/

#ifndef __UNDEF__
#define __UNDEF__

#include "Object.h"

namespace monash {

class Undef : public Object
{
private:
    Undef() {}
    virtual ~Undef() {}

public:
    static Undef* createInstance()
    {
        static Undef* u = NULL;
        if (NULL == u)
        {
            u = new Undef();
        }
        return u;
    }

public:
    virtual ::util::String toString()
    {
        return "#<undef>";
    }
    virtual ::util::String toStringValue()
    {
        return "#<undef>";
    }
    virtual int type() const
    {
        return Object::UNDEF;
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
        return "#<undef>";
    }
    virtual bool eqv(Object* o)
    {
        return o->isUndef();
    }
    virtual bool eq(Object* o)
    {
        return o->isUndef();
    }
};

}; // namespace monash

#endif // __UNDEF__

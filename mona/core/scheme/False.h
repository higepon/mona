/*!
    \file  False.h
    \brief

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/02 update:$Date$
*/

#ifndef __FALSE__
#define __FALSE__

#include "Object.h"

namespace monash {

class False : public Object
{
private:
    False() {}
    virtual ~False() {}

public:
    static False* createInstance()
    {
        static False* f =  NULL;
        if (NULL == f)
        {
            f = new False();
        }
        return f;
    }

public:
    virtual ::util::String toString()
    {
        return "#f";
    }
    virtual ::util::String toStringValue()
    {
        return "#f";
    }
    virtual int type() const
    {
        return Object::FALSE;
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
        return "#f";
    }
    virtual bool eqv(Object* o)
    {
        return o->isFalse();
    }
    virtual bool eq(Object* o)
    {
        return o->isFalse();
    }
};

}; // namespace monash

#endif // __FALSE__

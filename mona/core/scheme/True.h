/*!
    \file  True.h
    \brief

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/02 update:$Date$
*/

#ifndef __TRUE__
#define __TRUE__

#include "Object.h"

namespace monash {

class True : public Object
{
private:
    True() {}
    virtual ~True() {}

public:
    static True* createInstance()
    {
        static True* t = NULL;
        if (NULL == t)
        {
            t = new True();
        }
        return t;
    }

public:
    virtual ::util::String toString()
    {
        return "#t";
    }
    virtual ::util::String toStringValue()
    {
        return "#t";
    }
    virtual int type() const
    {
        return Object::TRUE;
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
        return "#t";
    }
    virtual bool eqv(Object* o)
    {
        return o->isTrue();
    }
    virtual bool eq(Object* o)
    {
        return o->isTrue();
    }
};

}; // namespace monash

#endif // __TRUE__

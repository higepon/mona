/*!
    \file  Eof.h
    \brief

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/02 update:$Date$
*/

#ifndef __EOF__
#define __EOF__

#include "Object.h"

namespace monash {

class Eof : public Object
{
private:
    Eof() {}
    virtual ~Eof() {}

public:
    static Eof* createInstance()
    {
        static Eof* t = NULL;
        if (NULL == t)
        {
            t = new Eof();
        }
        return t;
    }

public:
    virtual ::util::String toString()
    {
        return "#<eof>";
    }
    virtual ::util::String toStringValue()
    {
        return "#<eof>";
    }
    virtual int type() const
    {
        return Object::SEOF;
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
        return "#<eof>";
    }
    virtual bool eqv(Object* o)
    {
        return o->isEof();
    }
    virtual bool eq(Object* o)
    {
        return o->isEof();
    }
};

}; // namespace monash

#endif // __EOF__

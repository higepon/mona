/*!
    \file  Set.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/13 update:$Date$
*/

#ifndef __SET_H__
#define __SET_H__

#include "scheme.h"

namespace monash {

class Set : public Object
{
public:
    Set(Variable* variable, Object* expression, uint32_t lineno = 0) : variable_(variable),
                                                                       expression_(expression),
                                                                       lineno_(lineno)
    {
    }

    virtual ~Set() {}

public:
    virtual ::util::String toString()
    {
        return "#<syntax set!>";
    }
    virtual int type() const
    {
        return Object::SET;
    }
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const
    {
        return "#<syntax set!>";
    }
    virtual uint32_t lineno() const
    {
        return lineno_;
    }
    virtual bool eqv() const
    {
        return false;
    }
    virtual bool eq()  const
    {
        return false;
    }

protected:
    Variable* variable_;
    Object* expression_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __SET_H__

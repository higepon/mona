/*!
    \file  Continuation.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __CONTINUATION_H__
#define __CONTINUATION_H__

#include "Object.h"
#include "util/Vector.h"
extern "C" {
#include "libcont/cont.h"
}
#include "procedures/Procedure.h"

namespace monash {

class DynamicWind
{
public:
    DynamicWind(Object* before, Object* thunk, Object* after) : before(before), thunk(thunk), after(after) {}
    virtual ~DynamicWind() {}
    Object* before;
    Object* thunk;
    Object* after;
};

class Continuation : public Object
{
public:
    Continuation(uint32_t lineno = 0);
    virtual ~Continuation();

public:
    virtual ::util::String toString();
    virtual ::util::String toStringValue();
    virtual int type() const { return Object::CONTINUATION; }
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "continuation"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv(Object* o);
    virtual bool eq(Object* o);
    virtual bool equal(Object* o);
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);

    Cont cont;
    Objects* callAruguments;
    DynamicWind* dynamicWind;
protected:

    uint32_t lineno_;
};

}; // namespace monash

#endif // __CONTINUATION_H__

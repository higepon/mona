/*!
    \file  Begin.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __BEGIN_H__
#define __BEGIN_H__

#include "Object.h"

namespace monash {

class Begin : public Object
{
public:
    Begin(Objects* actions, uint32_t lineno = 0);
    virtual ~Begin();

public:
    virtual ::util::String toString();
    virtual int type() const { return Object::BEGIN; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "begin"; }
    virtual Object* eval(Environment* env);
    virtual Objects* actions() const { return actions_;}
    virtual bool eqv() const { return false; }
    virtual bool eq()  const { return false; }

protected:
    Objects* actions_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __BEGIN_H__

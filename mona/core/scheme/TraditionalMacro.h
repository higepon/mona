/*!
    \file  TraditionalMacro.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __TRADITIONAL_MACRO_H__
#define __TRADITIONAL_MACRO_H__

#include "scheme.h"

namespace monash {

class TraditionalMacro : public Object
{
public:
    TraditionalMacro(Object* procedure, uint32_t lineno = 0);
    virtual ~TraditionalMacro();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "if"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual Object* apply(Cons* arguments, Environment* environment);
    virtual bool eqv() const;
    virtual bool eq()  const;

protected:
    Object* procedure_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __TRADITIONAL_MACRO_H__

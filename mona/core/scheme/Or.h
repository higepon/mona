/*!
    \file  Or.h
    \brief

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/

#ifndef __OR_H__
#define __OR_H__

#include "scheme.h"

namespace monash {

class Or : public Object
{
public:
    Or(Cons* cons, uint32_t lineno = 0);
    virtual ~Or();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "or"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv() const;
    virtual bool eq()  const;

protected:
    Cons* cons_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __OR_H__

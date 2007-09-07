/*!
    \file  QuasiQuote.h
    \brief

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/

#ifndef __QUASI_QUAOTE_H__
#define __QUASI_QUAOTE_H__

#include "scheme.h"

namespace monash {

class QuasiQuote : public Object
{
public:
    QuasiQuote(Object* object, uint32_t lineno = 0);
    virtual ~QuasiQuote();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual Object* evalIter(Object* object, Environment* env);
    virtual ::util::String typeString() const { return "quasiquote"; }
    virtual uint32_t lineno() const { return lineno_; }
    virtual bool eqv() const;
    virtual bool eq()  const;

    virtual Object* cloneSexp(Object* o);

protected:
    Object* object_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __QUASI_QUAOTE_H__

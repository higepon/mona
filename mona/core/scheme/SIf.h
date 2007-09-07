/*!
    \file  SIf.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __SPECIALIF_H__
#define __SPECIALIF_H__

#include "scheme.h"

namespace monash {

class SIf : public Object
{
public:
    SIf(Object* predicate, Object* consequent, Object* alternative, uint32_t lineno = 0);
    virtual ~SIf();

public:
    virtual ::util::String toString();
    virtual int type() const;
    virtual Object* eval(Environment* env);
    virtual ::util::String typeString() const { return "if"; }
    virtual uint32_t lineno() const { return lineno_; }
    Object* predicate() const { return predicate_; }
    Object* consequent() const { return consequent_; }
    Object* alternative() const { return alternative_; }
    virtual bool eqv() const;
    virtual bool eq()  const;

protected:
    Object* predicate_;
    Object* consequent_;
    Object* alternative_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __SPECIALIF_H__

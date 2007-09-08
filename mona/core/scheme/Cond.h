/*!
    \file  Cond.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __COND_H__
#define __COND_H__

#include "Begin.h"
#include "Number.h"
#include "SIf.h"
#include "util/Pair.h"

namespace monash {

typedef ::util::Pair<Object*, Objects*> Clause;
typedef ::util::Vector<Clause*> Clauses;

class Cond : public Object
{
public:
    Cond(Clauses* clauses, Objects* elseActions, uint32_t lineno = 0);
    virtual ~Cond();

public:
    virtual ::util::String toString();
    virtual int type() const { return Object::COND; }
    virtual Object* eval(Environment* env);
    virtual uint32_t lineno() const { return lineno_; }
    virtual ::util::String typeString() const { return "cond"; }
    virtual Objects* elseActions() const { return elseActions_;}
    virtual Clauses* clauses() const { return clauses_; }
    virtual Object* expand();

protected:
    virtual Object* expandInternal(int i);
    Clauses* clauses_;
    Objects* elseActions_;
    uint32_t lineno_;
};

}; // namespace monash

#endif // __COND_H__

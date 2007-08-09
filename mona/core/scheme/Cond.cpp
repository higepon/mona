/*!
    \file   Cond.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Cond.h"

using namespace util;
using namespace monash;

Cond::Cond(Clauses* clauses, Objects* elseActions, uint32_t lineno /* = 0 */) : clauses_(clauses), elseActions_(elseActions), lineno_(lineno)
{
}

Cond::~Cond()
{
}

String Cond::toString()
{
    return String("cond");
}

int Cond::type() const
{
    return Object::COND;
}

Object* Cond::expand()
{
    return expandInternal(0);
}

Object* Cond::expandInternal(int i)
{
    if (i == clauses_->size())
    {
        if (NULL != elseActions_)
        {
            Object* begin = new Begin(elseActions_);SCM_ASSERT(begin); return begin;
        }
        else
        {
            Object* f = g_false; SCM_ASSERT(f); return f;
        }
    }
    Clause* clause = clauses_->get(i);
    Object* specialif = new SpecialIf(clause->first, new Begin(clause->second), expandInternal(i + 1));
    SCM_ASSERT(specialif);
    return specialif;
}

Object* Cond::eval(Environment* env)
{
    Object* specialif = expand();
    return specialif->eval(env);
}

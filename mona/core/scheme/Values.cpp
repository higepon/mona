/*!
    \file   Values.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Values.h"
#include "scheme.h"

using namespace util;
using namespace monash;

Values::Values(Objects* values, uint32_t lineno) : values_(values), lineno_(lineno)
{
}

Values::~Values()
{
}

String Values::toString()
{
    String ret = "";
    for (int i = 0; i < values_->size(); i++)
    {
        ret += values_->get(i)->toStringValue();
        ret += "\n";
    }
    return ret;
}

int Values::type() const
{
    return Object::VALUES;
}

Object* Values::eval(Environment* env)
{
    return this;
}

bool Values::eqv(Object* o)
{
    if (!o->isValues()) return false;
    Values* p = (Values*)o;
    return this == p;
}

bool Values::eq(Object* o)
{
    return eqv(o);
}


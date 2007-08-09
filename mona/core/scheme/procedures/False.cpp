/*!
    \file   False.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "False.h"
#include "primitive_procedures.h"
using namespace util;
using namespace monash;

False::False(uint32_t lineno) : lineno_(lineno)
{
}

False::~False()
{
}

::util::String False::toString()
{
    return "#f";
}

Object* False::eval(Environment* env)
{
    return this;
}

Object* False::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool False::eqv(Object* o)
{
    return o->isFalse();
}

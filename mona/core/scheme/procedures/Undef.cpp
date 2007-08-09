/*!
    \file   Undef.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Undef.h"

using namespace util;
using namespace monash;

Undef::Undef()
{
}

Undef::~Undef()
{
}

::util::String Undef::toString()
{
    return "#<undef>";
}

Object* Undef::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().data());
    return NULL;
}

Object* Undef::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool Undef::eqv(Object* o)
{
    return o->isUndef();
}


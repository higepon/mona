/*!
    \file   True.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "True.h"

using namespace util;
using namespace monash;

True::True()
{
}

True::~True()
{
}

::util::String True::toString()
{
    return "#t";
}

Object* True::eval(Environment* env)
{
    return this;
}

Object* True::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool True::eqv(Object* o)
{
    return o->isTrue();
}


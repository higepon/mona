/*!
    \file   TraditionalMacro.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/18 update:$Date$
*/
#include "TraditionalMacro.h"

using namespace util;
using namespace monash;

TraditionalMacro::TraditionalMacro(Object* procedure, uint32_t lineno) : procedure_(procedure)
{
}

TraditionalMacro::~TraditionalMacro()
{
}

String TraditionalMacro::toString()
{
    return "#<syntax traditional macro>";
}

int TraditionalMacro::type() const
{
    return Object::TRADITIONAL_MACRO;
}

Object* TraditionalMacro::eval(Environment* env)
{
    return this;
}

bool TraditionalMacro::eqv() const
{
    return false;
}

bool TraditionalMacro::eq() const
{
    return false;
}

Object* TraditionalMacro::apply(Cons* arguments, Environment* environment)
{
    Object* form = Kernel::apply(procedure_, arguments, environment, false);
    return Kernel::eval(form, environment);
}

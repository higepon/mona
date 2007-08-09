/*!
    \file   Variable.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Variable.h"
#include "Environment.h"

using namespace util;
using namespace monash;

Variable::Variable(const String& name, uint32_t lineno /* = 0 */) : name_(name), lineno_(lineno)
{
}

Variable::~Variable()
{
}

String Variable::toString()
{
    return "variable: " + name_;
}

String Variable::toStringValue()
{
    return name_;
}

int Variable::type() const
{
    return Object::VARIABLE;
}

Object* Variable::eval(Environment* env)
{
    return env->lookupVariableValue(this);
}

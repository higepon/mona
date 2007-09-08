/*!
    \file   Definition.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Definition.h"

using namespace util;
using namespace monash;

Definition::Definition(Variable* variable, Object* value, uint32_t lineno /* = 0 */) : variable_(variable), value_(value), lineno_(lineno)
{
}

Definition::~Definition()
{
}

String Definition::toString()
{
    return variable_->toString() + ": " + value_->toString();
}

Object* Definition::eval(Environment* env)
{
    env->defineVariable(variable_, value_->eval(env));
    return variable_;
}

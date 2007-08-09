/*!
    \file   Assignment.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Assignment.h"

using namespace util;
using namespace monash;

Assignment::Assignment(Variable* variable, Object* value) : variable_(variable), value_(value)
{
}

Assignment::~Assignment()
{
}

String Assignment::toString()
{
    return variable_->toString() + ": " + value_->toString();
}

int Assignment::type() const
{
    return Object::ASSIGNMENT;
}

Object* Assignment::eval(Environment* env)
{
    return value_;
}


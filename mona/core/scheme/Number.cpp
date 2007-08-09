/*!
    \file   Number.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Number.h"

using namespace util;
using namespace monash;

Number::Number(int value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

Number::~Number()
{
}

String Number::toString()
{
    snprintf(buf_, 63, "%d", value_);
    return String(buf_);
}

String Number::toStringValue()
{
    snprintf(buf_, 63, "%d", value_);
    return String(buf_);
}

int Number::type() const
{
    return Object::NUMBER;
}

int Number::value() const
{
    return value_;
}

Object* Number::eval(Environment* env)
{
    return this;
}

bool Number::eqv(Object* o)
{
    if (o->type() != Object::NUMBER) return false;
    Number* n = (Number*)o;
    return n->value() == value();
}

bool Number::eq(Object* o)
{
    return false;
}

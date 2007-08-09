/*!
    \file   Nil.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Nil.h"

using namespace util;
using namespace monash;

Nil::Nil(uint32_t lineno /* = 0 */) : lineno_(lineno)
{
}

Nil::~Nil()
{
}

String Nil::toString()
{
    return "()";
}

String Nil::toStringValue()
{
    return "()";
}

int Nil::type() const
{
    return Object::NIL;
}

Object* Nil::eval(Environment* env)
{
    return this;
}

bool Nil::eqv(Object* o)
{
    return o->isNil();
}

bool Nil::eq(Object* o)
{
    return eqv(o);
}

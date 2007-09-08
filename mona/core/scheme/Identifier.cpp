/*!
    \file   Identifier.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Identifier.h"

using namespace util;
using namespace monash;

Identifier::Identifier(const String& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

Identifier::~Identifier()
{
}

String Identifier::toString()
{
    return text();
}

String Identifier::toStringValue()
{
    return text();
}

Object* Identifier::eval(Environment* env)
{
    return this;
}

bool Identifier::eqv(Object* o)
{
    if (!o->isIdentifier()) return false;
    Identifier* r = (Identifier*)o;
    return this->toStringValue() == r->toStringValue();
}

bool Identifier::eq(Object* o)
{
    return eqv(o);
}

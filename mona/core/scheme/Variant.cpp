/*!
    \file   Variant.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Variant.h"

using namespace util;
using namespace monash;

Variant::Variant(void* data, uint32_t lineno /* = 0 */) : data_(data), lineno_(lineno)
{
}

Variant::~Variant()
{
}

String Variant::toString()
{
    return "#<variant>";
}

String Variant::toStringValue()
{
    return toString();
}

int Variant::type() const
{
    return Object::VARIANT;
}

Object* Variant::eval(Environment* env)
{
    return this;
}

bool Variant::eqv(Object* o)
{
    return o->isVariant();
}

bool Variant::eq(Object* o)
{
    return eqv(o);
}

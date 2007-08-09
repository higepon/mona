/*!
    \file   Eof.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Eof.h"

using namespace util;
using namespace monash;

Eof::Eof(uint32_t lineno /* = 0 */) : lineno_(lineno)
{
}

Eof::~Eof()
{
}

String Eof::toString()
{
    return "#<eof>";
}

String Eof::toStringValue()
{
    return toString();
}

int Eof::type() const
{
    return Object::SEOF;
}

Object* Eof::eval(Environment* env)
{
    return this;
}

bool Eof::eqv(Object* o)
{
    return o->isEof();
}

bool Eof::eq(Object* o)
{
    return eqv(o);
}

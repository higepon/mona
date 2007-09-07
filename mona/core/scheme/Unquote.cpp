/*!
    \file   Unquote.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/
#include "Unquote.h"

using namespace util;
using namespace monash;

Unquote::Unquote(Object* object, uint32_t lineno) : object_(object), lineno_(lineno)
{
}

Unquote::~Unquote()
{
}

String Unquote::toString()
{
    return "#<syntax unquote>";
}

int Unquote::type() const
{
    return Object::UNQUOTE;
}

Object* Unquote::eval(Environment* env)
{
    return Kernel::eval(object_, env);
}

bool Unquote::eqv() const
{
    return false;
}

bool Unquote::eq() const
{
    return false;
}

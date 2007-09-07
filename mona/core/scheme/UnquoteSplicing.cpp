/*!
    \file   UnquoteSplicing.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/
#include "UnquoteSplicing.h"

using namespace util;
using namespace monash;

UnquoteSplicing::UnquoteSplicing(Object* object, uint32_t lineno) : object_(object), lineno_(lineno)
{
}

UnquoteSplicing::~UnquoteSplicing()
{
}

String UnquoteSplicing::toString()
{
    return "#<syntax unquote-splicing>";
}

int UnquoteSplicing::type() const
{
    return Object::UNQUOTE_SPLICING;
}

Object* UnquoteSplicing::eval(Environment* env)
{
    return Kernel::eval(object_, env);
}

bool UnquoteSplicing::eqv() const
{
    return false;
}

bool UnquoteSplicing::eq() const
{
    return false;
}

/*!
    \file   Continuation.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Continuation.h"

using namespace util;
using namespace monash;

Continuation::Continuation(uint32_t lineno /* = 0 */) : dynamicWind(NULL), lineno_(lineno)
{
}

Continuation::~Continuation()
{
}

String Continuation::toString()
{
    return "continuation";
}

String Continuation::toStringValue()
{
    return toString();
}

int Continuation::type() const
{
    return Object::CONTINUATION;
}

Object* Continuation::eval(Environment* env)
{
    return this;
}

bool Continuation::eqv(Object* o)
{
    if (!o->isContinuation()) return false;
    Continuation* s = (Continuation*)o;
    return this == s;
}

bool Continuation::eq(Object* o)
{
    return false;
}

bool Continuation::equal(Object* o)
{
    return eqv(o);
}

// if this fail, see cont.c adjust next_stack= prev_stack - 0x1000;
Object* Continuation::apply(Objects* arguments, Environment* env)
{
    ARGC_SHOULD_BE_GT(0);
    callAruguments = as;
    cont_restore(&cont, 1);
    /* NOT REACHED */
    return NULL;
}

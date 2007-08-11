/*!
    \file   QuasiQuote.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/
#include "QuasiQuote.h"

using namespace util;
using namespace monash;

QuasiQuote::QuasiQuote(Objects* objects, uint32_t lineno) : objects_(objects), lineno_(lineno)
{
}

QuasiQuote::~QuasiQuote()
{
}

String QuasiQuote::toString()
{
    return "#<syntax quasiquote>";
}

int QuasiQuote::type() const
{
    return Object::QUASI_QUOTE;
}

Object* QuasiQuote::eval(Environment* env)
{
}

bool QuasiQuote::eqv() const
{
    return false;
}

bool QuasiQuote::eq() const
{
    return false;
}


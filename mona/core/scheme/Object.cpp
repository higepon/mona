/*!
    \file   Object.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Object.h"

using namespace monash;

Object::Object() : needEval(false), env(NULL)
{
}

Object::~Object()
{
}

Object* Object::apply(Objects* arguments, Environment* env)
{
    RAISE_ERROR(0, "unknown procedure [%s]", toString().data());
    return NULL;
}

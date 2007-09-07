/*!
    \file   Set.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/13 update:$Date$
*/
#include "Set.h"

using namespace util;
using namespace monash;

Object* Set::eval(Environment* env)
{
    env->setVaribale(variable_, expression_->eval(env));
    return SCM_UNDEF;
}

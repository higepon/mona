/*!
    \file   Let.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Let.h"

using namespace util;
using namespace monash;

Let::Let(Objects* body, Variables* variables, Cons* values, uint32_t lineno) : body_(body), variables_(variables), values_(values), lineno_(lineno)
{
}

Let::~Let()
{
}

String Let::toString()
{
    return String("let");
}

int Let::type() const
{
    return Object::LET;
}

Application* Let::expand()
{
    Lambda* lambda = new Lambda(body_, variables_, false);SCM_ASSERT(lambda);
    Application* application = new Application(lambda, values_); SCM_ASSERT(application); return application;
}

Object* Let::eval(Environment* env)
{
    Object* application = expand();
    return Kernel::evalTailOpt(application, env);
}

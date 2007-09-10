/*!
    \file   TraditionalMacro.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/18 update:$Date$
*/
#include "TraditionalMacro.h"

using namespace util;
using namespace monash;

TraditionalMacro::TraditionalMacro(Object* procedure, uint32_t lineno) : procedure_(procedure)
{
}

TraditionalMacro::~TraditionalMacro()
{
}

String TraditionalMacro::toString()
{
    return "#<syntax traditional macro>";
}

int TraditionalMacro::type() const
{
    return Object::TRADITIONAL_MACRO;
}

Object* TraditionalMacro::eval(Environment* env)
{
    return this;
}

bool TraditionalMacro::eqv() const
{
    return false;
}

bool TraditionalMacro::eq() const
{
    return false;
}

Object* TraditionalMacro::apply(Cons* arguments, Environment* environment)
{
//    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    Object* form = Kernel::applyFullEvaled(procedure_, arguments, environment, false);
//    printf("%s %s:%d %s\n", __func__, __FILE__, __LINE__, form->toString().data());fflush(stdout);// debug
//     if (form->needEval) {
//         form = Kernel::evalTailOpt(form, form->env);
//     }
    // form require compile, so, use eval instead of evalTailOpt
    Object* ret = Kernel::eval(form, environment);
//    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    return ret;
}

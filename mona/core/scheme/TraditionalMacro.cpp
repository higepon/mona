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
//     Objects* as = Kernel::listOfValues(arguments, environment);
//     for (int i = 0; i < as->size(); i++)
//     {
//         printf("as=%s\n", as->get(i)->toStringValue().data());
//     }
//    printf("proc type=%s\n", procedure_->typeString().data());
//    printf("macro apply arguments=%s\n", arguments->toString().data());
    Object* form = Kernel::apply(procedure_, arguments, environment, false);
//    printf("type=%s\n", form->typeString().data());
//    printf("type=%s\n", form->toString().data());
    return Kernel::eval(form, environment);//environment->translator2().translate(form)->eval(environment);
//     Object* evalFunc = (new Variable("eval"))->eval(environment);
//     Object* evaluated = NULL;
//     printf("type=%s\n", form->typeString().data());
//     printf("type=%s\n", form->toString());
//     SCM_EVAL(evalFunc, environment, evaluated, form);
//     return evaluated;
}

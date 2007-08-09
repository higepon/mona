/*!
    \file   LetAsterisk.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "LetAsterisk.h"

using namespace util;
using namespace monash;

LetAsterisk::LetAsterisk(Objects* body, Variables* variables, Objects* values, uint32_t lineno) : body_(body)
                                                                                                , variables_(variables)
                                                                                                , values_(values)
                                                                                                , lineno_(lineno)
{
}

LetAsterisk::~LetAsterisk()
{
}

String LetAsterisk::toString()
{
    return String("let");
}

int LetAsterisk::type() const
{
    return Object::LET_ASTERISK;
}

Object* LetAsterisk::expand()
{
    Variables* variables = new Variables;SCM_ASSERT(variables);
    variables->add(variables_->get(0));
    Objects* values = new Objects;SCM_ASSERT(values);
    values->add(values_->get(0));
    Object* let = new Let(expandInternal(1, 1), variables, values); SCM_ASSERT(let); return let;
}

Objects* LetAsterisk::expandInternal(int variablesIndex, int valuesIndex)
{
    if (variablesIndex == variables_->size() || valuesIndex == values_->size())
    {
        return body_;
    }
    Variables* variables = new Variables;SCM_ASSERT(variables);
    variables->add(variables_->get(variablesIndex));
    Objects* values = new Objects;SCM_ASSERT(values);
    values->add(values_->get(valuesIndex));
    Let* let = new Let(expandInternal(variablesIndex + 1, valuesIndex + 1), variables, values);SCM_ASSERT(let);
    Objects* body = new Objects;SCM_ASSERT(body);
    body->add(let);
    return body;
}

Object* LetAsterisk::eval(Environment* env)
{
    Object* application = expand();
    return application->eval(env);
}

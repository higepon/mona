/*!
  \file   main.cpp
  \brief  scheme

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/06/16 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "Number.h"
#include "String.h"

using namespace monash;

typedef void Env;


void display(Object* exp)
{
    printf("%s", exp->toString().c_str());
}

bool isSelfEvaluating(Object* exp)
{
    if (exp->type() == Object::NUMBER || exp->type() == Object::STRING)
    {
        return true;
    }
    return false;
}

bool is_variable(Object* exp)
{
    return true;
}

bool is_quated(Object* exp)
{
    return true;
}

bool is_assignment(Object* exp)
{
    return true;
}

bool is_definition(Object* exp)
{
    return true;
}

bool is_if(Object* exp)
{
    return true;
}

bool is_lambda(Object* exp)
{
    return true;
}

bool is_begin(Object* exp)
{
    return true;
}

bool is_cond(Object* exp)
{
    return true;
}

bool is_application(Object* exp)
{
    return true;
}

Object* lookup_variable_value(Object* exp, Env* env)
{
    return NULL;
}

Object* text_of_quotation(Object* exp)
{
    return NULL;
}

Object* eval_assignment(Object* exp, Env* env)
{
    return NULL;
}

Object* eval_definition(Object* exp, Env* env)
{
    return NULL;
}

Object* eval_if(Object* exp, Env* env)
{
    return NULL;
}


Object* eval(Object* exp, Env* env)
{
    if (isSelfEvaluating(exp))
    {
        return exp;
    }
    else if (is_variable(exp))
    {
        return lookup_variable_value(exp, env);
    }
    else if (is_quated(exp))
    {
        return text_of_quotation(exp);
    }
    else if (is_assignment(exp))
    {
        return eval_assignment(exp, env);
    }
    else if (is_definition(exp))
    {
        return eval_definition(exp, env);
    }
    else if (is_if(exp))
    {
        return eval_if(exp, env);
    }
    else if (is_lambda(exp))
    {
    }
    else if (is_begin(exp))
    {
    }
    else if (is_cond(exp))
    {
    }
    else if (is_application(exp))
    {
    }
    else
    {
        fprintf(stderr, "Unknown expression type -- EVAL %s ", exp);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    Number exp(7);
    display(eval(&exp, NULL));
    return 0;
}

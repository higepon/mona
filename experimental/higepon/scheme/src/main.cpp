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

#include <string>

using namespace std;

typedef void Env;
typedef std::string Exp;

bool is_self_evaluating(const Exp& exp)
{
    return true;
}

bool is_variable(const Exp& exp)
{
    return true;
}

bool is_quated(const Exp& exp)
{
    return true;
}

bool is_assignment(const Exp& exp)
{
    return true;
}

bool is_definition(const Exp& exp)
{
    return true;
}

bool is_if(const Exp& exp)
{
    return true;
}

bool is_lambda(const Exp& exp)
{
    return true;
}

bool is_begin(const Exp& exp)
{
    return true;
}

bool is_cond(const Exp& exp)
{
    return true;
}

bool is_application(const Exp& exp)
{
    return true;
}

void* lookup_variable_value(const Exp& exp, Env* env)
{
    return NULL;
}

void* text_of_quotation(const Exp& exp)
{
    return NULL;
}

void* eval_assignment(const Exp& exp, Env* env)
{
    return NULL;
}

void* eval_definition(const Exp& exp, Env* env)
{
    return NULL;
}

void* eval_if(const Exp& exp, Env* env)
{
    return NULL;
}


void* eval(const Exp& exp, Env* env)
{
    if (is_self_evaluating(exp))
    {
        /* fix me */
        //return exp;
        return NULL;
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
    return 0;
}

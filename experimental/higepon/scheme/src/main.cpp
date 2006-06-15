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

bool is_self_evaluating(const string& exp)
{
    return true;
}

bool is_variable(const string& exp)
{
    return true;
}

bool is_quated(const std::string& exp)
{
    return true;
}

bool is_assignment(const std::string& exp)
{
    return true;
}

bool is_definition(const std::string& exp)
{
    return true;
}

bool is_if(const std::string& exp)
{
    return true;
}

bool is_lambda(const std::string& exp)
{
    return true;
}

bool is_begin(const std::string& exp)
{
    return true;
}

bool is_cond(const std::string& exp)
{
    return true;
}

bool is_application(const std::string& exp)
{
    return true;
}

int eval(const std::string& exp, void* env /* fix me */)
{
    if (is_self_evaluating(exp))
    {
    }
    else if (is_variable(exp))
    {
    }
    else if (is_quated(exp))
    {
    }
    else if (is_assignment(exp))
    {
    }
    else if (is_definition(exp))
    {
    }
    else if (is_if(exp))
    {
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

/*!
    \file   Procedure.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Procedure.h"

using namespace util;
using namespace monash;

Procedure::Procedure(Lambda* lambda, Environment* env, bool isMacro, uint32_t lineno)
    : env_(env), isExtendableParameter_(false), isExtendableParameters_(false), isMacro_(isMacro), lineno_(lineno)
{
    body_ = lambda->body();
    parameters_ = new Variables();SCM_ASSERT(parameters_);
    Variables* lparameters = lambda->parameters();
    isExtendableParameters_ = lambda->isExtendableParameters();
    isExtendableParameter_  = lambda->isExtendableParameter();
    for (int i = 0; i < lparameters->size(); i++)
    {
        parameters_->add(lparameters->get(i));
    }
}

Procedure::~Procedure()
{
}

String Procedure::toString()
{
    return "#<closure>";
}

int Procedure::type() const
{
    return Object::PROCEDURE;
}

Object* Procedure::eval(Environment* env)
{
    return this;
}

bool Procedure::eqv() const
{
    return false;
}

bool Procedure::eq() const
{
    return false;
}


Object* Procedure::apply(Objects* arguments, Environment* environment, bool evalArguments /* = true */)
{
    Objects* as = evalArguments? Kernel::listOfValues(arguments, environment) : arguments;
    Environment* e = env()->clone();
    Variables* params = parameters();
    if (isExtendableParameter_)
    {
        Objects* args = new Objects;
        if (0 == as->size())
        {
            args->add(SCM_NIL);
        }
        else
        {
            Cons* start = new Cons(SCM_NIL, SCM_NIL);
            Cons* p = start;
            for (int j = 0; j < as->size(); j++)
            {
                p->setCar(as->get(j));
                if (j != as->size() -1)
                {
                    Cons* tmp = new Cons;
                    p->setCdr(tmp);
                    p = tmp;
                }
            }
            args->add(start);
        }
        e->extend(params, args); // doubt? we need copy?
    }
    else if (isExtendableParameters_)
    {
        Objects* args = new Objects;
        for (int i = 0; i < params->size(); i++)
        {
            Variable* v = params->get(i);
            if (v->name() == ".")
            {
                args->add(SCM_NIL); // . => nil
                if ((params->size() - 1) > as->size())
                {
                    args->add(SCM_NIL);
                    break;
                }

                Cons* start = new Cons(SCM_NIL, SCM_NIL);
                Cons* p = start;
                for (int j = i; j < as->size(); j++)
                {
                    p->setCar(as->get(j));
                    if (j != as->size() -1)
                    {
                        Cons* tmp = new Cons(SCM_NIL, SCM_NIL);
                        p->setCdr(tmp);
                        p = tmp;
                    }
                }
                args->add(start);
                break;
            }
            else
            {
                args->add(as->get(i));
            }
        }

        e->extend(params, args); // doubt? we need copy?
    }
    else
    {
        int params_length = params->size();
        int args_length = as->size();
        if (params_length != args_length)
        {
            for (int i = 0; i < args_length; i++)
            {
                printf("as=%s\n", as->get(i)->toString().data());
            }
            RAISE_ERROR(lineno(), "procedure got %d argument(s), but required %d", args_length, params_length);
            return NULL;
        }
        e->extend(params, as); // doubt? we need copy?
    }
    Object* ret = NULL;
    if (isMacro_)
    {
        ret = Kernel::evalSequence(body(), e);
        ret = Kernel::eval(ret, e);
    }
    else
    {
//         ret = Kernel::evalSequence(body(), e);
        Objects* exps = body();
        for (int i = 0; i < exps->size() - 1; i++)
        {
            Kernel::evalTailOpt(exps->get(i), e);
        }
        ret = exps->get(exps->size() - 1);

        ret->needEval = true;
        ret->env      = e;
    }
    return ret;
}

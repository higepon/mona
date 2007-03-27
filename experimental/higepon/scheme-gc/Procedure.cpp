#include "Procedure.h"

using namespace util;
using namespace monash;

Procedure::Procedure(Lambda* lambda, Environment* env, uint32_t lineno)
    : env_(env), lineno_(lineno), isExtendableParameter_(false), isExtendableParameters_(false)
{
    body_ = lambda->body();
    parameters_ = new Variables();ASSERT(parameters_);
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
//    delete parameters_;
}

String Procedure::toString()
{
    return "procedure : " + body_->get(0)->toString();
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


Object* Procedure::apply(Objects* arguments, Environment* environment)
{
    Objects* as = Kernel::listOfValues(arguments, environment);
//     Objects* as;
//     moge(arguments, 0, NULL, environment, &as);
    Environment* e = env()->clone();
    Variables* params = parameters();
    if (isExtendableParameter_)
    {
        Objects* args = new Objects;
        SExp* sexp = new SExp(SExp::SEXPS);
#if 0
        Quote* nil = new Quote(sexp);
#endif
        Pair* start = new Pair(SCM_NIL, SCM_NIL);
        Pair* p = start;
        for (int j = 0; j < as->size(); j++)
        {
            p->setCar(as->get(j));
            if (j != as->size() -1)
            {
                Pair* tmp = new Pair(NULL, SCM_NIL);
                p->setCdr(tmp);
                p = tmp;
            }
        }
        args->add(start);
        e->extend(params, args); // doubt? we need copy?
    }
    else if (isExtendableParameters_)
    {
        Objects* args = new Objects;
        SExp* sexp = new SExp(SExp::SEXPS);
#if 0
        Quote* nil = new Quote(sexp);
#else
#endif
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

                Pair* start = new Pair(SCM_NIL, SCM_NIL);
                Pair* p = start;
                for (int j = i; j < as->size(); j++)
                {
                    p->setCar(as->get(j));
                    if (j != as->size() -1)
                    {
                        Pair* tmp = new Pair(SCM_NIL, SCM_NIL);
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
        uint32_t params_length = params->size();
        uint32_t args_length = as->size();
        if (params_length != args_length)
        {
            RAISE_ERROR(lineno(), "procedure got %d argument(s), but required %d", args_length, params_length);
            return NULL;
        }
        e->extend(params, as); // doubt? we need copy?
    }
    return Kernel::evalSequence(body(), e);
}

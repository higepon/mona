#include "Procedure.h"

using namespace monash;

Procedure::Procedure(Lambda* lamba, Environment* env, uint32_t lineno) : env_(env), lineno_(lineno), extendableParameters_(false)
{
    body_ = lamba->body();
    parameters_ = new Variables();ASSERT(parameters_);
    Variables* lparameters = lamba->parameters();
    for (Variables::const_iterator it = lparameters->begin(); it != lparameters->end(); ++it)
    {
        parameters_->push_back(*it);
        if ((*it)->name() == ".")
        {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            extendableParameters_ = true;
        }
    }
}

Procedure::~Procedure()
{
//    delete parameters_;
}

std::string Procedure::toString()
{
    return "procedure : " + body_->at(0)->toString();
}

int Procedure::type() const
{
    return Object::PROCEDURE;
}

Object* Procedure::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
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
    Environment* e = env()->clone();
    Variables* params = parameters();
    if (extendableParameters_)
    {
        Objects* args = new Objects;
        SExp* sexp = new SExp(SExp::SEXPS);
        Quote* nil = new Quote(sexp);
        for (Variables::size_type i = 0; i < params->size(); i++)
        {
            Variable* v = params->at(i);
            if (v->name() == ".")
            {
                args->push_back(nil); // . => nil
                if ((params->size() - 1) > as->size())
                {
                    args->push_back(nil);
                    break;
                }

                Pair* start = new Pair(NULL, nil);
                Pair* p = start;
                for (Objects:: size_type j = i; j < as->size(); j++)
                {
                    p->setCar(as->at(j));
                    if (j != as->size() -1)
                    {
                        Pair* tmp = new Pair(NULL, nil);
                        p->setCdr(tmp);
                        p = tmp;
                    }
                }
                args->push_back(start);
                break;
            }
            else
            {
                args->push_back(as->at(i));
            }
        }
        e->extend(params, args); // doubt? we need copy?
    }
    else
    {
        e->extend(params, as); // doubt? we need copy?
    }
    return Kernel::evalSequence(body(), e);
}

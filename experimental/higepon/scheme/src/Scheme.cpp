#include "Scheme.h"

using namespace monash;

Scheme::Scheme()
{
}

Scheme::~Scheme()
{
}

Object* Scheme::evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::const_iterator it = exps->begin(); it != exps->end(); ++it)
    {
        Object* o = (*it);
        ret = o->eval(env);
    }
    return ret;
}

Objects* Scheme::listOfValues(Objects* objects, Environment* env)
{
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
}

Object* Scheme::apply(Object* procedure, Objects* arguments, Environment* env)
{
    if (procedure->isCompoundProcedure())
    {
        Objects* as = Scheme::listOfValues(arguments, env);
        Procedure* p = (Procedure*)procedure;
        Environment* e = p->env()->clone();

        e->extend(p->parameters(), as); // doubt? we need copy?
        return Scheme::evalSequence(p->body(), e);
    }
    else if (procedure->isPrimitiveProcedure())
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments, env);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().c_str());
        return NULL;
    }
}

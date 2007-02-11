#include "Kernel.h"

using namespace monash;

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
}

Object* Kernel::evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::const_iterator it = exps->begin(); it != exps->end(); ++it)
    {
        Object* o = (*it);
        ret = o->eval(env);
    }
    return ret;
}

Objects* Kernel::listOfValues(Objects* objects, Environment* env)
{
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
}

Object* Kernel::apply(Object* procedure, Objects* arguments, Environment* env)
{
    if (procedure->isCompoundProcedure())
    {
        Procedure* p = (Procedure*)procedure;
        return p->apply(arguments, env);
    }
    else if (procedure->isPrimitiveProcedure())
    {
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;
        return p->apply(arguments, env);
    }
    else if (procedure->isContinuation())
    {
        Continuation* cont = (Continuation*)procedure;
        return cont->apply(arguments, env);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().c_str());
        return NULL;
    }
}

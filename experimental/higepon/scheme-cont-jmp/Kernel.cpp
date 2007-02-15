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

void Kernel::makeListOfValues(Objects* objects, uint32_t i, Argument* prev, Environment* environment, Objects** values)
{
    Objects::size_type size = objects->size();
    if (0 == size)
    {
        *values = new Objects;
        return;
    }
    Argument arg;
    arg.object = objects->at(i)->eval(environment);
    arg.prev   = prev;
    if (i == objects->size() - 1)
    {
        *values = new Objects;
        for (Argument* a = &arg; a != NULL; a = a->prev)
        {
            (*values)->push_back(a->object);
        }
        std::reverse((*values)->begin(), (*values)->end());
        return;
    }
    return makeListOfValues(objects, i + 1, &arg, environment, values);
}

Objects* Kernel::listOfValues(Objects* objects, Environment* env)
{
// if you use this, continuation doesn't work.
#if 0
    Objects* result = new Objects;ASSERT(result);
    for (Objects::const_iterator it = objects->begin(); it != objects->end(); ++it)
    {
        result->push_back((*it)->eval(env));
    }
    return result;
#else
    Objects* result;
    makeListOfValues(objects, 0, NULL, env, &result);
    return result;
#endif
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

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
    for (int i = 0; i < exps->size(); i++)
    {
        Object* o = exps->get(i);
        ret = o->eval(env);
    }
    return ret;
}

void Kernel::makeListOfValues(Objects* objects, int i, Argument* prev, Environment* environment, Objects** values)
{
    uint32_t size = objects->size();
    if (0 == size)
    {
        *values = new Objects;
        return;
    }
    Argument arg;
    arg.object = objects->get(i)->eval(environment);
    arg.prev   = prev;
    if (i == objects->size() - 1)
    {
        *values = new Objects;
        for (Argument* a = &arg; a != NULL; a = a->prev)
        {
            (*values)->add(a->object);
        }
        (*values)->reverse();
        return;
    }
    return makeListOfValues(objects, i + 1, &arg, environment, values);
}

Objects* Kernel::listOfValues(Objects* objects, Environment* env)
{
    Objects* result;
    makeListOfValues(objects, 0, NULL, env, &result);
    return result;
}

Object* Kernel::apply(Object* procedure, Objects* arguments, Environment* env)
{
    if (arguments->size() == 1 && arguments->get(0)->isValues())
    {
        Values* vs = (Values*)arguments->get(0);
        arguments = vs->values();
    }
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
    else if (procedure->isSRegexp())
    {
        SRegexp* r = (SRegexp*)procedure;
        return r->apply(arguments, env);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().data());
        return NULL;
    }
}

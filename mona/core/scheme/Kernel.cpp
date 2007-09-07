/*!
    \file   Kernel.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
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

Object* Kernel::eval(Object* sexp, Environment* environment)
{
    return environment->translator().translate(sexp)->eval(environment);
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
//    printf("makeListOfValues %s to be evaled\n", objects->get(i)->toString().data());
    arg.object = Kernel::eval(objects->get(i), environment)->eval(environment);
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

Object* Kernel::apply(Object* procedure, Cons* operands, Environment* env, bool evalArguments /* = true */)
{
//    printf("Kernel::apply procedure = %s type=%s\n", procedure->toString().data(), procedure->typeString().data());
//    printf("Kernel::apply operands = %s\n", operands ? operands->toString().data() : "NULL");

    // traditional macro
    //    operands should be S Expression
    if (procedure->isTraditionaMacro())
    {
//        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        TraditionalMacro* macro = (TraditionalMacro*)procedure;
        return macro->apply(operands, env);
    }

    Objects* arguments = new Objects;
    SCM_ASSERT(arguments);
    if (operands != NULL)
    {
        operands->toObjects(arguments);
    }

//     for (int i = 0; i < arguments->size(); i++)
//     {
//         printf("args[%d] = %s\n", i, arguments->get(i)->toString().data());
//     }

    if (arguments->size() == 1 && arguments->get(0)->isValues())
    {
        Values* vs = (Values*)arguments->get(0);
        arguments = vs->values();
    }
    if (procedure->isCompoundProcedure())
    {
        Procedure* p = (Procedure*)procedure;
        return p->apply(arguments, env, evalArguments);
    }
    else if (procedure->isPrimitiveProcedure())
    {
//        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        PrimitiveProcedure* p = (PrimitiveProcedure*)procedure;

        return p->apply(arguments, env, evalArguments);
    }
    else if (procedure->isContinuation())
    {
        Continuation* cont = (Continuation*)procedure;
        return cont->apply(arguments, env, evalArguments);
    }
    else if (procedure->isSRegexp())
    {
//        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        SRegexp* r = (SRegexp*)procedure;
        return r->apply(arguments, env, evalArguments);
    }
    else if (procedure->isSRegMatch())
    {
        SRegMatch* r = (SRegMatch*)procedure;
        return r->apply(arguments, env, evalArguments);
    }
    else
    {
        RAISE_ERROR(procedure->lineno(), "unknown procedure [%s]", procedure->toString().data());
        return NULL;
    }
}

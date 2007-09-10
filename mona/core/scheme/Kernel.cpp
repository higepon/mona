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
        ret = Kernel::evalTailOpt(o, env);
    }
    return ret;
}

Object* Kernel::compile(Object* sexp, Environment* environment)
{
    return g_translator->translate(sexp);//environment->translator().translate(sexp);
}

Object* Kernel::eval(Object* sexp, Environment* environment)
{
    return Kernel::evalTailOpt(Kernel::compile(sexp, environment), environment);
}

Object* Kernel::evalTailOpt(Object* sexp, Environment* environment)
{
//    register char* esp asm ("%esp");
//    if (sexp->toString() == "Application : variable: hoge") printf("evalTailOpt Enter[%s] %x\n", sexp->toString().data(), esp);
    Object* exp = sexp;
    exp->env = environment;
eval:
//    if (sexp->toString() == "Application : variable: hoge")  printf("evalTailOpt eval start[%s]\n", sexp->toString().data());
    exp = exp->eval(exp->env);
    if (exp->needEval)
    {
        exp->needEval = false;
//        if (sexp->toString() == "Application : variable: hoge")printf("  evalTailOpt eval once more[%s]\n", sexp->toString().data());
        goto eval;
    }
//    if (sexp->toString() == "Application : variable: hoge") printf("evalTailOpt eval end[%s]\n", sexp->toString().data());
    return exp;
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
    arg.object = Kernel::evalTailOpt(Kernel::eval(objects->get(i), environment), environment);
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



// tail optimization!
//   if you need return Object to be full evaluted, use applyFullEvaled
Object* Kernel::apply(Object* procedure, Cons* operands, Environment* env, bool evalArguments /* = true */)
{
    // traditional macro
    //    operands should be S Expression
    if (procedure->isTraditionaMacro())
    {
        TraditionalMacro* macro = (TraditionalMacro*)procedure;
        return macro->apply(operands, env);
    }

    Objects* arguments = new Objects;
    SCM_ASSERT(arguments);
    if (operands != NULL)
    {
        operands->toObjects(arguments);
    }

    if (arguments->size() == 1 && arguments->get(0)->isValues())
    {
        Values* vs = (Values*)arguments->get(0);
        arguments = vs->values();
    }
    if (procedure->isCompoundProcedure())
    {
        Procedure* p = (Procedure*)procedure;
        Object* o =  p->apply(arguments, env, evalArguments);
        return o;
    }
    else if (procedure->isPrimitiveProcedure())
    {
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

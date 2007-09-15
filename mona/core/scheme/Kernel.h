/*!
    \file  Kernel.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef _KERNEL_
#define _KERNEL_

#include "Object.h"
#include "Procedure.h"

namespace monash {

#ifdef USE_BOEHM_GC
typedef struct Argument : public gc
#else
typedef struct Argument
#endif
{
    Object* object;
    Argument* prev;
} Argument;

    class Translator;

class Kernel
{
public:
    Kernel();
    virtual ~Kernel();

    static Object* compile(Object* sexp, Environment* environment);
    static Object* eval(Object* sexp, Environment* environment);
    static Object* evalTailOpt(Object* sexp, Environment* environment);
    static Object* evalSequence(Objects* exps, Environment* env);
    static Objects* listOfValues(Objects* objects, Environment* env);
    static Object* apply(Object* procedure, Cons* operands, Environment* env, bool evalArguments = true);
    static inline Object* applyFullEvaled(Object* procedure, Cons* operands, Environment* env, bool evalArguments = true)
    {
        Object* ret = apply(procedure, operands, env, evalArguments);
        if (ret->needEval)
        {
            ret = evalTailOpt(ret, ret->env);
        }
        return ret;
    }
    static Object* doContinuation();
    static void makeListOfValues(Objects* objects, int i, Argument* prev, Environment* environment, Objects** values);
};

}; // namespace monash

#endif // _KERNEL_

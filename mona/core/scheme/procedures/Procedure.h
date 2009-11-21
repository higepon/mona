/*!
    \file  Procedure.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __PRIMITIVE_PROCEDURE_PROCEDURE_H__
#define __PRIMITIVE_PROCEDURE_PROCEDURE_H__

#include "PrimitiveProcedure.h"
#include "Kernel.h"
#include "scheme.h"
#ifdef USE_MONA_GC
#include "../gc/gc.h"
#endif

#define ARGV(i) (as->get(i))
#define ARGC    (as->size())


#define RETURN_BOOLEAN(condition) \
    if ((condition))              \
    {                             \
        return SCM_TRUE;          \
    }                             \
    else                          \
    {                             \
        return SCM_FALSE;         \
    }

#define CAST(o, type, to)                                                        \
    if (!o->is##type())                                                          \
    {                                                                            \
        RAISE_ERROR(o->lineno(), "%s got wrong argument. got %s but required " #type ""  \
                    , toString().data(), o->typeString().data());                \
                                                                                 \
    }                                                                            \
    type* to = (type*)o;

#define CAST_RETURN_FALSE(o, type, to)                                           \
    if (!o->is##type())                                                          \
    {                                                                            \
        return SCM_FALSE;                                                        \
    }                                                                            \
    type* to = (type*)o;

#define ARGC_SHOULD_BE(n)                                                        \
    Objects* as = evalArguments ? Kernel::listOfValues(arguments, env) : arguments;          \
    if (ARGC != n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0                            \
                    , "%s got %d argument(s), but required %d"                   \
                    , toString().data(), ARGC, n);                               \
    }

#define ARGC_SHOULD_BE_GT(n)                                                     \
    Objects* as = evalArguments ? Kernel::listOfValues(arguments, env) : arguments;          \
    if (ARGC <= n)                                                               \
    {                                                                            \
        RAISE_ERROR(ARGC > 0 ? ARGV(0)->lineno() : 0                             \
                    , "%s got %d argument(s), but required at least %d"          \
                    , toString().data(), ARGC, n + 1);                           \
    }

#define ARGC_SHOULD_BE_GT_RETURN_FALSE(n)                                        \
    Objects* as = evalArguments ? Kernel::listOfValues(arguments, env) : arguments;          \
    if (ARGC <= n)                                                               \
    {                                                                            \
        return SCM_FALSE;                                                        \
    }


#define ARGC_SHOULD_BE_BETWEEN(m, n)                                                         \
    Objects* as = evalArguments ? Kernel::listOfValues(arguments, env) : arguments;          \
    if (ARGC < m || ARGC > n)                                                                \
    {                                                                                        \
        RAISE_ERROR(ARGC > 0 ? ARGV(0)->lineno() : 0                                         \
                    , "%s got %d argument(s), but required between %d and %d arguments."     \
                    , toString().data(), ARGC, m, n);                                        \
    }


#define PROCEDURE(ClassName, name)                                               \
    Object* ClassName::apply(Objects* arguments, Environment* env, bool evalArguments /* = true */)

#endif // __PRIMITIVE_PROCEDURE_PROCEDURE_H__

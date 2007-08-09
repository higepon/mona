/*!
    \file   VectorProcedures.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "procedures/Procedure.h"
#include "primitive_procedures.h"
using namespace std;
using namespace monash;

PROCEDURE(VectorP, "vector?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isVector());
}

PROCEDURE(MakeVector, "make-vector")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGC == 1)
    {
        CAST(ARGV(0), Number, n);
        return new Vector(n->value(), n->lineno());
    }
    else
    {
        CAST(ARGV(0), Number, n)
        return new Vector(n->value(), ARGV(1), n->lineno());
    }
}

PROCEDURE(CreateVector, "vector")
{
    Objects* as = Kernel::listOfValues(arguments, env);
    return new Vector(as, as->isEmpty() ? 0 : ARGV(0)->lineno());
}

PROCEDURE(VectorLength, "vector-length")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Vector, v);
#ifdef USE_MONA_GC
    return new(false) Number(v->size(), v->lineno());
#else
    return new Number(v->size(), v->lineno());
#endif
}

PROCEDURE(VectorRef, "vector-ref")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Vector, v);
    CAST(ARGV(1), Number, n);
    Object* ret = v->get(n->value());
    if (ret == NULL)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong index", toString().data());
        return NULL;
    }
    return ret;
}

PROCEDURE(VectorSet, "vector-set!")
{
    ARGC_SHOULD_BE(3);
    CAST(ARGV(0), Vector, v);
    CAST(ARGV(1), Number, n);
    bool isOK = v->set(n->value(), ARGV(2));
    if (!isOK)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong arguments", toString().data());
    }
    return SCM_UNDEF;
}

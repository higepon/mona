#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

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
        CAST(ARGV(0), Number, n)
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
    return new Vector(as, as->empty() ? 0 : ARGV(0)->lineno());
}

PROCEDURE(VectorLength, "vector-length")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Vector, v);
    return new Number(v->size(), v->lineno());
}

PROCEDURE(VectorRef, "vector-ref")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Vector, v);
    CAST(ARGV(1), Number, n);
    Object* ret = v->get(n->value());
    if (ret == NULL)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong index", toString().c_str());
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
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong arguments", toString().c_str());
    }
    return new Undef();
}

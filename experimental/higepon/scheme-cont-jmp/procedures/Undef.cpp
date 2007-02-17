#include "Undef.h"

using namespace monash;

using namespace monash::util;

Undef::Undef()
{
}

Undef::~Undef()
{
}

::util::String Undef::toString()
{
    return "#<undef>";
}

Object* Undef::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().data());
    return NULL;
}

Object* Undef::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool Undef::eqv(Object* o)
{
    return o->isUndef();
}


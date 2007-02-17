#include "False.h"

using namespace monash;

using namespace monash::util;

False::False(uint32_t lineno) : lineno_(lineno)
{
}

False::~False()
{
}

::util::String False::toString()
{
    return "#f";
}

Object* False::eval(Environment* env)
{
    return this;
}

Object* False::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool False::eqv(Object* o)
{
    return o->isFalse();
}

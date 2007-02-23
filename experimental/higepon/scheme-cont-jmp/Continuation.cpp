#include "Continuation.h"

using namespace util;
using namespace monash;

Continuation::Continuation(uint32_t lineno /* = 0 */) : dynamicWind(NULL), lineno_(lineno)
{
}

Continuation::~Continuation()
{
}

String Continuation::toString()
{
    return "continuation";
}

String Continuation::toStringValue()
{
    return toString();
}

int Continuation::type() const
{
    return Object::CONTINUATION;
}

Object* Continuation::eval(Environment* env)
{
    return this;
}

bool Continuation::eqv(Object* o)
{
    if (!o->isContinuation()) return false;
    Continuation* s = (Continuation*)o;
    return this == s;
}

bool Continuation::eq(Object* o)
{
    return false;
}

bool Continuation::equal(Object* o)
{
    return eqv(o);
}

Object* Continuation::apply(Objects* arguments, Environment* env)
{
    ARGC_SHOULD_BE_GT(0);
    callAruguments = as;
    cont_restore(&cont, 1);
    /* NOT REACHED */
    return NULL;
}

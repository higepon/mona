#include "Nil.h"

using namespace util;
using namespace monash;

Nil::Nil(uint32_t lineno /* = 0 */) : lineno_(lineno)
{
}

Nil::~Nil()
{
}

String Nil::toString()
{
    return "nil";
}

String Nil::toStringValue()
{
    return "nil";
}

int Nil::type() const
{
    return Object::NIL;
}

Object* Nil::eval(Environment* env)
{
    return this;
}

bool Nil::eqv(Object* o)
{
    return o->isNil();
}

bool Nil::eq(Object* o)
{
    return eqv(o);
}

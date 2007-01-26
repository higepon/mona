#include "False.h"

using namespace monash;

using namespace std;

False::False(uint32_t lineno) : lineno_(lineno)
{
}

False::~False()
{
}

std::string False::toString()
{
    return "#f";
}

Object* False::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* False::apply(Objects* arguments, Environment* env, Object* parent)
{
    return this;
}

bool False::eqv(Object* o)
{
    return o->isFalse();
}

#include "Undef.h"

using namespace monash;

using namespace std;

Undef::Undef()
{
}

Undef::~Undef()
{
}

std::string Undef::toString()
{
    return "#<undef>";
}

Object* Undef::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* Undef::apply(Objects* arguments, Environment* env, Object* parent, Object* applicationca)
{
    return this;
}

bool Undef::eqv(Object* o)
{
    return o->isUndef();
}


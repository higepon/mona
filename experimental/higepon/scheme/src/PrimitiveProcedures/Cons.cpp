#include "Cons.h"

using namespace monash;

Cons::Cons()
{
}

Cons::~Cons()
{
}

std::string Cons::toString()
{
    return "procedure:cons";
}

Object* Cons::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* Cons::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "cons got %d arguments, but required %d", as->size(), 2);
        return NULL;
    }
    return new Pair(as->at(0), as->at(1));
}

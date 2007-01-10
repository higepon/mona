#include "Eqv.h"

using namespace monash;

using namespace std;

Eqv::Eqv()
{
}

Eqv::~Eqv()
{
}

std::string Eqv::toString()
{
    return "procedure:eqv";
}

Object* Eqv::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* Eqv::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "eqv? got %d arguments, but required %d", as->size(), 2);
        return NULL;
    }
    Object* o1 = as->at(0);
    Object* o2 = as->at(1);
    return new Number(o1->eqv(o2) ? 1 : 0); // todo true/false
}

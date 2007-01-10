#include "StringP.h"

using namespace monash;

StringP::StringP()
{
}

StringP::~StringP()
{
}

std::string StringP::toString()
{
    return "procedure:number?";
}

Object* StringP::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* StringP::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() != 1)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "string? got %d arguments, but required %d", as->size(), 1);        printf("string? got error");
        return NULL;
    }
    Object* o = as->at(0);
    if (o->type() == Object::STRING)
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

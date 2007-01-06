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
    printf("don't eval me");
    return NULL;
}

Object* StringP::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() != 1)
    {
        printf("string? got error");
        return NULL;
    }
    Object* o = as->at(0);
    return o->type() == Object::STRING ? new Number(1) : new Number(0);
}

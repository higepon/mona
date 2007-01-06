#include "NumberP.h"

using namespace monash;

NumberP::NumberP()
{
}

NumberP::~NumberP()
{
}

std::string NumberP::toString()
{
    return "procedure:number?";
}

Object* NumberP::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* NumberP::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() != 1)
    {
        printf("number? got error");
        return NULL;
    }
    Object* o = as->at(0);
    return o->type() == Object::NUMBER ? new Number(1) : new Number(0);
}

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
    printf("don't eval me");
    return NULL;
}

Object* Cons::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() != 2)
    {
        printf("cons need only two arguments");
        return NULL;
    }
    return new Pair(as->at(0), as->at(1));
}

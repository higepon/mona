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

Object* Cons::apply(Objects* arguments)
{
    if (arguments->size() != 2)
    {
        printf("cons need only two arguments");
        return NULL;
    }
    return new Pair(arguments->at(0), arguments->at(1));
}

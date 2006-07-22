#include "Cdr.h"

using namespace monash;

Cdr::Cdr()
{
}

Cdr::~Cdr()
{
}

std::string Cdr::toString()
{
    return "procedure:cdr";
}

Object* Cdr::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* Cdr::apply(Objects* arguments)
{
    if (arguments->size() != 1)
    {
        printf("cdr need only one argument");
        return NULL;
    }
    Object* o = arguments->at(0);
    if (o->type() != Object::PAIR)
    {
        printf("cdr argument need to be pair");
        return NULL;
    }
    Pair* p = (Pair*)o;
    return p->second();
}

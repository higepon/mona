#include "Multiply.h"

using namespace monash;

Multiply::Multiply()
{
}

Multiply::~Multiply()
{
}

std::string Multiply::toString()
{
    return "procedure:+";
}

Object* Multiply::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* Multiply::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    int total = 1;
    for (Objects::const_iterator it = as->begin(); it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->type() == Object::NUMBER)
        {
            Number* n = (Number*)o;
            total *= n->value();
        }
    }
    return new Number(total);
}

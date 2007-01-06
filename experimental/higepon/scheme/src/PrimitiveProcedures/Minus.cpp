#include "Minus.h"

using namespace monash;

Minus::Minus()
{
}

Minus::~Minus()
{
}

std::string Minus::toString()
{
    return "procedure:+";
}

Object* Minus::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* Minus::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() == 0 || as->at(0)->type() != Object::NUMBER)
    {
        fprintf(stderr, "minus error\n");
        return NULL;
    }
    Number* n = (Number*)as->at(0);
    if (as->size () == 1) return new Number(-1 * n->value());

    int total = n->value();
    for (Objects::const_iterator it = as->begin() + 1; it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->type() == Object::NUMBER)
        {
            Number* nn = (Number*)o;
            total -= nn->value();
        }
    }
    return new Number(total);
}

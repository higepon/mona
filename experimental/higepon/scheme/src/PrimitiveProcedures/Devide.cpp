#include "Devide.h"

using namespace monash;

Devide::Devide()
{
}

Devide::~Devide()
{
}

std::string Devide::toString()
{
    return "procedure:+";
}

Object* Devide::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* Devide::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() == 0 || as->at(0)->type() != Object::NUMBER)
    {
        fprintf(stderr, "minus error\n");
        return NULL;
    }
    Number* n = (Number*)as->at(0);

    int total = n->value();
    for (Objects::const_iterator it = as->begin() + 1; it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->type() == Object::NUMBER)
        {
            Number* nn = (Number*)o;
            if (nn->value() == 0)
            {
                fprintf(stderr, "zero division\n");
                return NULL;
            }
            total /= nn->value();
        }
    }
    return new Number(total);
}

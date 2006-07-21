#include "Plus.h"

using namespace monash;

Plus::Plus()
{
}

Plus::~Plus()
{
}

std::string Plus::toString()
{
    return "procedure:+";
}

Object* Plus::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* Plus::apply(Objects* arguments)
{
    int total = 0;
    for (Objects::iterator it = arguments->begin(); it != arguments->end(); it++)
    {
        Object* o = (*it);
        if (o->type() == Object::NUMBER)
        {
            Number* n = (Number*)o;
            total += n->value();
        }
    }
    return new Number(total);
}

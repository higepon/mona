#include "NumberEqual.h"

using namespace monash;

NumberEqual::NumberEqual()
{
}

NumberEqual::~NumberEqual()
{
}

std::string NumberEqual::toString()
{
    return "procedure:=";
}

Object* NumberEqual::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* NumberEqual::apply(Objects* arguments)
{
    if (arguments->size() != 2) return new Number(0);
    Object* o1 = arguments->at(0);
    Object* o2 = arguments->at(1);
    if (o1->type() != Object::NUMBER || o2->type() != Object::NUMBER) return new Number(0);
    Number* n1 = (Number*)o1;
    Number* n2 = (Number*)o2;
    if (n1->value() == n2->value()) return new Number(1);
    return new Number(0);
}

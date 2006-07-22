#include "Car.h"

using namespace monash;

Car::Car()
{
}

Car::~Car()
{
}

std::string Car::toString()
{
    return "procedure:car";
}

Object* Car::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* Car::apply(Objects* arguments)
{
    if (arguments->size() != 1)
    {
        printf("car need only one argument");
        return NULL;
    }
    Object* o = arguments->at(0);
    if (o->type() != Object::PAIR)
    {
        printf("car argument need to be pair");
        return NULL;
    }
    Pair* p = (Pair*)o;
    return p->first();
}

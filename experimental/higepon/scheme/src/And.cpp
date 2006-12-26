#include "And.h"

using namespace monash;

And::And(Objects* objects) : objects_(objects)
{
}

And::~And()
{
}

std::string And::toString()
{
    return "and";
}

int And::type() const
{
    return Object::AND;
}

Object* And::eval(Environment* env)
{
    for (Objects::iterator it = objects_->begin(); it != objects_->end(); it++)
    {
        Object* o = (*it);
        if (!isTrue(o->eval(env)))
        {
            Object* number = new Number(0); // false
            ASSERT(number);
            return number;
        }
    }
    return objects_->at(objects_->size() - 1); // true
}

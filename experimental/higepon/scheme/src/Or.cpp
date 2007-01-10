#include "Or.h"

using namespace monash;

Or::Or(Objects* objects) : objects_(objects)
{
}

Or::~Or()
{
}

std::string Or::toString()
{
    return "and";
}

int Or::type() const
{
    return Object::OR;
}

Object* Or::eval(Environment* env)
{
    for (Objects::size_type i = 0; i < objects_->size(); i++)
    {
        Object* o = objects_->at(i);
        Object* evalResult = o->eval(env);
        if (isTrue(evalResult))
        {
            return evalResult;
        }
    }
    Object* f = new False();ASSERT(f); return f;
}

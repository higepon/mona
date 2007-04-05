#include "Object.h"

using namespace monash;

Object::Object()
{
}

Object::~Object()
{
}

Object* Object::apply(Objects* arguments, Environment* env)
{
    RAISE_ERROR(0, "unknown procedure [%s]", toString().data());
    return NULL;
}

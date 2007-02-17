#include "Object.h"

using namespace monash;

Object::Object()
{
}

Object::~Object()
{
    printf("%s", __func__);fflush(stdout);
}

Object* Object::apply(Objects* arguments, Environment* env)
{
    RAISE_ERROR(0, "unknown procedure [%s]", toString().data());
    return NULL;
}

#include "Object.h"

using namespace monash;

Object::Object() : parent(NULL)
{
}

Object::~Object()
{
    printf("%s", __func__);fflush(stdout);
}


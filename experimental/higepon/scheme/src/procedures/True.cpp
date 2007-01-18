#include "True.h"

using namespace monash;

using namespace std;

True::True()
{
}

True::~True()
{
}

std::string True::toString()
{
    return "#t";
}

Object* True::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* True::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool True::eqv(Object* o)
{
    return o->isTrue();
}


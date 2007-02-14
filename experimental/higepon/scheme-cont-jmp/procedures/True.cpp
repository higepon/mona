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
    return this;
}

Object* True::apply(Objects* arguments, Environment* env)
{
    return this;
}

bool True::eqv(Object* o)
{
    return o->isTrue();
}


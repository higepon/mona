#include "scheme.h"

using namespace monash;

Begin::Begin(Objects* actions) : actions_(actions)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
}

Begin::~Begin()
{
}

std::string Begin::toString()
{
    Objects::iterator o = actions_->begin();
    return "begin: " + (*o)->toString();
}

int Begin::type() const
{
    return Object::BEGIN;
}

Object* Begin::eval(Environment* env)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    return evalSequence(this->actions(), env);
}

#include "scheme.h"

using namespace monash;

Begin::Begin(Objects* actions) : actions_(actions)
{
}

Begin::~Begin()
{
}

std::string Begin::toString()
{
    std::string ret = "begin:";
    for (Objects::const_iterator it = actions_->begin(); it != actions_->end(); ++it)
    {
        ret += "[" + (*it)->toString() + "]";
    }
    return ret;
}

int Begin::type() const
{
    return Object::BEGIN;
}

Object* Begin::eval(Environment* env)
{
    return evalSequence(this->actions(), env);
}

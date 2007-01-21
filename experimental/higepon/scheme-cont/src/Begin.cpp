#include "scheme.h"

using namespace monash;

Begin::Begin(Objects* actions, uint32_t lineno) : actions_(actions), lineno_(lineno)
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
    return Scheme::evalSequence(this->actions(), env);
}

bool Begin::eqv() const
{
    return false;
}

bool Begin::eq() const
{
    return false;
}

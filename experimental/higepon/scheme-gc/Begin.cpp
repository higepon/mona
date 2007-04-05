#include "scheme.h"
using namespace util;
using namespace monash;

Begin::Begin(Objects* actions, uint32_t lineno) : actions_(actions), lineno_(lineno)
{
}

Begin::~Begin()
{
}

String Begin::toString()
{
    String ret = "begin:";
    for (int i = 0; i < actions_->size(); i++)
    {
        ret += "[" + actions_->get(i)->toString() + "]";
    }
    return ret;
}

int Begin::type() const
{
    return Object::BEGIN;
}

Object* Begin::eval(Environment* env)
{
    return Kernel::evalSequence(this->actions(), env);
}

bool Begin::eqv() const
{
    return false;
}

bool Begin::eq() const
{
    return false;
}

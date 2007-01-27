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
    return Kernel::evalSequence(this->actions(), env);
}


// exp fix me todo
#include <algorithm>
Object* Begin::getContinuation(Object* object)
{
    Objects* actions = new Objects;
    Begin* ret = new Begin(actions, lineno());
    actions->push_back(*(actions_->begin() + 2));
    Objects* z = new Objects;
    z->push_back(ret);
    Variables* v = new Variables;
    v->push_back(new Variable("hige"));
    return new Lambda(z, new Variables, lineno());
#if 0
    Objects* actions = new Objects;
    Objects::iterator target = find(actions_->begin(), actions_->end(), object);
    if (target == actions_->end())
    {
        RAISE_ERROR(3, "getContinuation");
    }

    Begin* ret = new Begin(actions, lineno());
    for (Objects::const_iterator it = target + 1 ; it != actions_->end(); ++it)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        actions_->push_back(*it);
    }
    return new Lambda(ret, new Variables, lineno());
#endif
}

bool Begin::eqv() const
{
    return false;
}

bool Begin::eq() const
{
    return false;
}

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
    for (int i = 0; i < actions_->size(); i++)
    {
        ret += "[" + actions_->get(i)->toString() + "]";
    }
//     for (Objects::const_iterator it = actions_->begin(); it != actions_->end(); ++it)
//     {
//         ret += "[" + (*it)->toString() + "]";
//     }
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
    actions->add(actions_->get(2));
    Objects* z = new Objects;
    z->add(ret);
    Variables* v = new Variables;
    v->add(new Variable("hige"));
    return new Lambda(z, new Variables, lineno());
}

bool Begin::eqv() const
{
    return false;
}

bool Begin::eq() const
{
    return false;
}

#include "Variable.h"

using namespace monash;

Variable::Variable(Object* object) : object_(object)
{
}

Variable::~Variable()
{
}

std::string Variable::toString()
{
    return object_->toString();
}

int Variable::type() const
{
    return Object::VARIABLE;
}

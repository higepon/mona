#include "Definition.h"

using namespace monash;

Definition::Definition(Variable* variable, Object* value, uint32_t lineno /* = 0 */) : variable_(variable), value_(value), lineno_(lineno)
{
}

Definition::~Definition()
{
}

std::string Definition::toString()
{
    return variable_->toString() + ": " + value_->toString();
}

int Definition::type() const
{
    return Object::DEFINITION;
}

Object* Definition::eval(Environment* env)
{
    env->defineVariable(variable_, value_->eval(env));
    return variable_;
}

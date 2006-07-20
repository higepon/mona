#include "Definition.h"

using namespace monash;

Definition::Definition(Variable* variable, Object* value) : variable_(variable), value_(value)
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
    return new Quote("OK"); // different from SICP's
}

// Object* Definition::eval(Environment* env)
// {
//     return this;
// }

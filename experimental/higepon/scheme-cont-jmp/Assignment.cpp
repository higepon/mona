#include "Assignment.h"

using namespace util;
using namespace monash;

Assignment::Assignment(Variable* variable, Object* value) : variable_(variable), value_(value)
{
}

Assignment::~Assignment()
{
}

String Assignment::toString()
{
    return variable_->toString() + ": " + value_->toString();
}

int Assignment::type() const
{
    return Object::ASSIGNMENT;
}

Object* Assignment::eval(Environment* env)
{
    return value_;
}


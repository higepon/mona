#include "Assignment.h"

using namespace monash;

Assignment::Assignment(Variable* variable, Object* value) : variable_(variable), value_(value)
{
}

Assignment::~Assignment()
{
}

std::string Assignment::toString()
{
    return variable_->toString() + ": " + value_->toString();
}

int Assignment::type() const
{
    return Object::ASSIGNMENT;
}


#include "Variable.h"

using namespace monash;

Variable::Variable(const std::string& name) : name_(name)
{
}

Variable::~Variable()
{
}

std::string Variable::toString()
{
    return "variable: " + name_;
}

int Variable::type() const
{
    return Object::VARIABLE;
}

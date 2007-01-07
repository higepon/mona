#include "Variable.h"
#include "Environment.h"

using namespace monash;

Variable::Variable(const std::string& name, uint32_t lineno /* = 0 */) : name_(name), lineno_(lineno)
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

Object* Variable::eval(Environment* env)
{
    return env->lookupVariableValue(this);
}

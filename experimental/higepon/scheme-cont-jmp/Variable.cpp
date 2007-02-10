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

std::string Variable::toStringValue()
{
    return name_;
}

int Variable::type() const
{
    return Object::VARIABLE;
}

Object* Variable::eval(Environment* env)
{
//    if (name_ == "puga") printf("%s", env->toString().c_str());
    return env->lookupVariableValue(this);
}

#include "Variable.h"
#include "Environment.h"

using namespace util;
using namespace monash;

Variable::Variable(const String& name, uint32_t lineno /* = 0 */) : name_(name), lineno_(lineno)
{
}

Variable::~Variable()
{
}

String Variable::toString()
{
    return "variable: " + name_;
}

String Variable::toStringValue()
{
    return name_;
}

int Variable::type() const
{
    return Object::VARIABLE;
}

Object* Variable::eval(Environment* env)
{
//    if (name_ == "puga") printf("%s", env->toString().data());
    return env->lookupVariableValue(this);
}

#include "Application.h"
#include "Kernel.h"
#include "scheme.h"

using namespace util;
using namespace monash;

Application::Application(Object* function, Objects* arguments, uint32_t lineno) : function_(function), arguments_(arguments), lineno_(lineno)
{
}

Application::~Application()
{
}

String Application::toString()
{
    return "Application : " + function_->toString();
}

int Application::type() const
{
    return Object::APPLICATION;
}

Object* Application::eval(Environment* env)
{
    Object* procedure = function()->eval(env);

    if (!procedure->isCompoundProcedure() &&
        !procedure->isPrimitiveProcedure() &&
        !procedure->isContinuation() &&
        !procedure->isSRegexp() &&
        !procedure->isSRegMatch())
    {
        RAISE_ERROR(lineno(), "invalid application [%s]", procedure->toString().data());
    }

    Object* ret = Kernel::apply(procedure, arguments(), env);
    procedure = NULL;
    return ret;
}

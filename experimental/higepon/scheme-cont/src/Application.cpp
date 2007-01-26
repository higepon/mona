#include "Application.h"
#include "Kernel.h"
#include "scheme.h"

using namespace std;
using namespace monash;

Application::Application(Object* function, Objects* arguments, uint32_t lineno) : function_(function), arguments_(arguments), lineno_(lineno)
{
}

Application::~Application()
{
}

string Application::toString()
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
    if (procedure->type() != Object::PROCEDURE && procedure->type() != Object::PRIMITIVE_PROCEDURE)
    {
        RAISE_ERROR(lineno(), "invalid application [%s]", procedure->toString().c_str());
    }

    //Objects* as = arguments();
    return Kernel::apply(procedure, arguments(), env, parent);
}

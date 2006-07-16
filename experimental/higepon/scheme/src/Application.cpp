#include "Application.h"

using namespace std;
using namespace monash;

Application::Application(Object* function, Objects* arguments) : function_(function), arguments_(arguments)
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

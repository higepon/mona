#include "Application.h"
#include "scheme.h"

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

Object* Application::eval(Environment* env)
{
    Object* procedure =this->function()->eval(env);
    if (procedure->type() != Object::PROCEDURE && procedure->type() != Object::PRIMITIVE_PROCEDURE)
    {
        printf("error %s %s:%d[%d]\n", __func__, __FILE__, __LINE__, procedure->type());fflush(stdout);// debug
        exit(-1);
    }

    Objects* as = arguments();

//        for (Objects::const_iterator i = as->begin(); i != as->end(); ++i)
//         {
//             printf("!!!!!!!!!!!!!!!!!apply as %s\n",(*i)->toString().c_str());fflush(stdout);
//         }

    return apply(procedure, arguments(), env);

 
//    return apply(procedure, listOfValues(this->arguments(), env), env);
}

#include "Lambda.h"

using namespace monash;

Lambda::Lambda(Objects* body, Variables* parameters) : body_(body), parameters_(parameters)
{
    for (Objects::iterator it = body->begin(); it != body->end(); it++)
    {
        printf("lambda body %d\n", (*it)->type());
    }
}

Lambda::~Lambda()
{
}

std::string Lambda::toString()
{
    return "lambda : " + body_->at(0)->toString();
}

int Lambda::type() const
{
    return Object::LAMBDA;
}

Object* Lambda::eval(Environment* env)
{
    return new Procedure(this, env);
}

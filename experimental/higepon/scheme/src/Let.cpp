#include "Let.h"

using namespace monash;

Let::Let(Objects* body, Variables* variables, Objects* values) : body_(body), variables_(variables), values_(values)
{
}

Let::~Let()
{
}

std::string Let::toString()
{
    return std::string("let");
}

int Let::type() const
{
    return Object::LET;
}

Application* Let::expand()
{
    Lambda* lambda = new Lambda(body_, variables_);
    return new Application(lambda, values_);
}

Object* Let::eval(Environment* env)
{
    Object* application = expand();
    return application->eval(env);
}

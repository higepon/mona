#include "NamedLet.h"

using namespace util;
using namespace monash;

NamedLet::NamedLet(Objects* body, Variables* variables, Objects* values, String name, uint32_t lineno)
    : body_(body), variables_(variables), values_(values), name_(name), lineno_(lineno)
{
}

NamedLet::~NamedLet()
{
}

String NamedLet::toString()
{
    return String("named let");
}

int NamedLet::type() const
{
    return Object::NAMED_LET;
}

Application* NamedLet::expand()
{
    Variable* v = new Variable(name_, lineno());
    Definition* definition = new Definition(v, new Lambda(body_, variables_, false, lineno()));
    Application* application = new Application(v, values_, lineno());
    Objects* body = new Objects;
    body->add(definition);
    body->add(application);
    Lambda* lambda = new Lambda(body, new Variables(), false, lineno());
    Application* apps = new Application(lambda, new Objects());
    return apps;
}

Object* NamedLet::eval(Environment* env)
{
    return expand()->eval(env);
}

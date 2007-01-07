#include "Lambda.h"

using namespace monash;

Lambda::Lambda(Objects* body, Variables* parameters, uint32_t lineno) : body_(body), parameters_(parameters), lineno_(lineno)
{
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
    Object* procedure = new Procedure(this, env); ASSERT(procedure); return procedure;
}

#include "Lambda.h"

using namespace monash;
using namespace monash::util;

// lambda (x)     : normal parameter
// lambda x       : extendableParameter. x is list.
// lambda (x . y) : extendableParameters. y is list.
Lambda::Lambda(Objects* body, Variables* parameters, bool isExtendableParameter, uint32_t lineno)
    : body_(body), parameters_(parameters), isExtendableParameter_(isExtendableParameter), lineno_(lineno)
{
    for (int i = 0; i < parameters->size(); i++)
    {
        if (parameters->get(i)->name() == ".")
        {
            isExtendableParameters_ = true;
        }
    }
}

Lambda::~Lambda()
{
}

::util::String Lambda::toString()
{
    return "lambda : " + body_->get(0)->toString();
}

int Lambda::type() const
{
    return Object::LAMBDA;
}

Object* Lambda::eval(Environment* env)
{
    Object* procedure = new Procedure(this, env); ASSERT(procedure); return procedure;
}

bool Lambda::eqv() const
{
    return false;
}

bool Lambda::eq() const
{
    return false;
}


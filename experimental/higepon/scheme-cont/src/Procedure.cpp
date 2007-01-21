#include "Procedure.h"

using namespace monash;

Procedure::Procedure(Lambda* lamba, Environment* env, uint32_t lineno) : env_(env), lineno_(lineno)
{
    body_ = lamba->body();
    parameters_ = new Variables();ASSERT(parameters_);
    Variables* lparameters = lamba->parameters();
    for (Variables::const_iterator it = lparameters->begin(); it != lparameters->end(); ++it)
    {
        parameters_->push_back(*it);
    }
}

Procedure::~Procedure()
{
//    delete parameters_;
}

std::string Procedure::toString()
{
    return "procedure : " + body_->at(0)->toString();
}

int Procedure::type() const
{
    return Object::PROCEDURE;
}

Object* Procedure::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return this;
}

bool Procedure::eqv() const
{
    return false;
}

bool Procedure::eq() const
{
    return false;
}

#include "SpecialIf.h"

using namespace monash;

SpecialIf::SpecialIf(Object* predicate, Object* consequent, Object* alternative) : predicate_(predicate)
                                                                                 , consequent_(consequent)
                                                                                 , alternative_(alternative)
{
}

SpecialIf::~SpecialIf()
{
}

std::string SpecialIf::toString()
{
    return "if (" + predicate_->toString() + ") " + consequent_->toString() + ":" + alternative_->toString();
}

int SpecialIf::type() const
{
    return Object::IF;
}

Object* SpecialIf::eval(SpecialIf* specialIf, Environment* env)
{
    if (isTrue(::eval(specialIf->predicate(), env)))
    {
        return ::eval(specialIf->consequent(), env);
    }
    else
    {
        return ::eval(specialIf->alternative(), env);
    }
}

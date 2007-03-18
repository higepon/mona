#include "SpecialIf.h"

using namespace util;
using namespace monash;

SpecialIf::SpecialIf(Object* predicate, Object* consequent, Object* alternative, uint32_t lineno) : predicate_(predicate)
                                                                                                  , consequent_(consequent)
                                                                                                  , alternative_(alternative)
                                                                                                  , lineno_(lineno)
{
}

SpecialIf::~SpecialIf()
{
}

String SpecialIf::toString()
{
    return "if (" + predicate_->toString() + ") {" + consequent_->toString() + "}else{" + alternative_->toString() + "}";
}

int SpecialIf::type() const
{
    return Object::IF;
}

Object* SpecialIf::eval(Environment* env)
{
    if (!predicate()->eval(env)->isFalse())
    {
        return this->consequent()->eval(env);
    }
    else
    {
        if (this->alternative() != NULL)
        {
            return this->alternative()->eval(env);
        }
        else
        {
            return SCM_FALSE;
        }
    }
}

bool SpecialIf::eqv() const
{
    return false;
}

bool SpecialIf::eq() const
{
    return false;
}


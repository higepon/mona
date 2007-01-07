#include "SpecialIf.h"

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

std::string SpecialIf::toString()
{
    return "if (" + predicate_->toString() + ") {" + consequent_->toString() + "}else{" + alternative_->toString() + "}";
}

int SpecialIf::type() const
{
    return Object::IF;
}

Object* SpecialIf::eval(Environment* env)
{
    if (isTrue(this->predicate()->eval(env)))
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
            return new Number(0, this->predicate()->lineno()); // false
        }
    }
}

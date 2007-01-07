#include "Cond.h"

using namespace monash;

Cond::Cond(Clauses* clauses, Objects* elseActions, uint32_t lineno /* = 0 */) : clauses_(clauses), elseActions_(elseActions), lineno_(lineno)
{
}

Cond::~Cond()
{
}

std::string Cond::toString()
{
    return std::string("cond");
}

int Cond::type() const
{
    return Object::COND;
}

Object* Cond::expand()
{
    return expandInternal(clauses_->begin());
}

Object* Cond::expandInternal(Clauses::iterator it)
{
    if (it == clauses_->end())
    {
        if (NULL != elseActions_)
        {
            Object* begin = new Begin(elseActions_);ASSERT(begin); return begin;
        }
        else
        {
            Object* number = new Number(0); ASSERT(number); return number; // fix me
        }
    }
    Clause* clause = *it;
    Object* specialif = new SpecialIf(clause->first, new Begin(clause->second), expandInternal(++it)); ASSERT(specialif); return specialif;
}

Object* Cond::eval(Environment* env)
{
    Object* specialif = expand();
    return specialif->eval(env);
}

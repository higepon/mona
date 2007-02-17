#include "Cond.h"

using namespace monash;

Cond::Cond(Clauses* clauses, Objects* elseActions, uint32_t lineno /* = 0 */) : clauses_(clauses), elseActions_(elseActions), lineno_(lineno)
{
}

Cond::~Cond()
{
}

::util::String Cond::toString()
{
    return ::util::String("cond");
}

int Cond::type() const
{
    return Object::COND;
}

Object* Cond::expand()
{
    return expandInternal(0);
}

Object* Cond::expandInternal(int i)
{
    if (i == clauses_->size())
    {
        if (NULL != elseActions_)
        {
            Object* begin = new Begin(elseActions_);ASSERT(begin); return begin;
        }
        else
        {
            Object* f = new False(); ASSERT(f); return f;
        }
    }
    Clause* clause = clauses_->get(i);
    Object* specialif = new SpecialIf(clause->first, new Begin(clause->second), expandInternal(i + 1));
    ASSERT(specialif);
    return specialif;
}

Object* Cond::eval(Environment* env)
{
    Object* specialif = expand();
    return specialif->eval(env);
}

#include "Pair.h"

//using namespace util;
using namespace monash;

Pair::Pair(Object* car, Object* cdr, uint32_t lineno) : car_(car), cdr_(cdr), lineno_(lineno)
{
}

Pair::~Pair()
{
}

::util::String Pair::toString()
{
    return toStringInternal(false);
}

::util::String Pair::toStringInternal(bool inList /* = false */)
{
    ::util::String carString = car_ ? car_->toString() : "NULL";
    if (cdr_->isPair())
    {
        Pair* p = (Pair*)cdr_;
        ::util::String cdrString = p->toStringInternal(true);
        if (inList)
        {
            return carString + " " + cdrString;
        }
        else
        {
            if (carString == "quote")
            {
                return "\'" + cdrString;
            }
            else
            {
                return "(" + carString + " " + cdrString + ")";
            }
        }
    }
    else if (cdr_->isNil())
    {
        if (inList)
        {
            return carString;
        }
        else
        {
            return "(" + carString + ")";
        }
    }
    else
    {
        ::util::String cdrString = cdr_->toString();
        if (inList)
        {
            return carString + " . " + cdrString;
        }
        else
        {
            return "(" + carString + " . " + cdrString + ")";
        }
    }
}

::util::String Pair::toStringValue()
{
    ::util::String carString = car_ ? car_->toStringValue() : "NULL";
    ::util::String cdrString = cdr_ ? cdr_->toStringValue() : "NULL";
    return "(cons " + carString + " " + cdrString + ")";
}

int Pair::type() const
{
    return Object::PAIR;
}

Object* Pair::eval(Environment* env)
{
    return this;
}

bool Pair::eqv(Object* o)
{
    if (!o->isPair()) return false;
    Pair* p = (Pair*)o;
    return this == p;
}

bool Pair::eq(Object* o)
{
    return eqv(o);
}

void Pair::setCar(Object* o)
{
    car_ = o;
}

void Pair::setCdr(Object* o)
{
    cdr_ = o;
}

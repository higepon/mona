#include "Pair.h"

using namespace monash;
using namespace std;

Pair::Pair(Object* car, Object* cdr, uint32_t lineno) : car_(car), cdr_(cdr), lineno_(lineno)
{
}

Pair::~Pair()
{
}

std::string Pair::toString()
{
    return "pair(" + getCar()->toString() + ", " + getCdr()->toString() + ")";
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

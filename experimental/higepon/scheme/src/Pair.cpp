#include "Pair.h"

using namespace monash;
using namespace std;

Pair::Pair(Object* first, Object* second) : first_(first), second_(second)
{
}

Pair::~Pair()
{
}

std::string Pair::toString()
{
    return "pair(" + first_->toString() + ", " + second_->toString() + ")";
}

int Pair::type() const
{
    return Object::PAIR;
}

Object* Pair::eval(Environment* env)
{
    return this;
}

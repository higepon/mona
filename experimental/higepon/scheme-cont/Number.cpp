#include "Number.h"

using namespace monash;
using namespace std;

Number::Number(int value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

Number::~Number()
{
}

std::string Number::toString()
{
    snprintf(buf_, 63, "number: %d", value_);
    return string(buf_);
}

std::string Number::toStringValue()
{
    snprintf(buf_, 63, "%d", value_);
    return string(buf_);
}

int Number::type() const
{
    return Object::NUMBER;
}

int Number::value() const
{
    return value_;
}

Object* Number::eval(Environment* env)
{
    return this;
}

bool Number::eqv(Object* o)
{
    if (o->type() != Object::NUMBER) return false;
    Number* n = (Number*)o;
    return n->value() == value();
}

bool Number::eq(Object* o)
{
    return false;
}

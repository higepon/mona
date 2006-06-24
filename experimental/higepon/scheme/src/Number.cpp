#include "Number.h"

using namespace monash;

Number::Number(int value) : value_(value)
{
}

Number::~Number()
{
}

std::string Number::toString()
{
    char buf[32];
    snprintf(buf, 32, "%d", value_);
    return std::string(buf);
}

int Number::type() const
{
    return Object::NUMBER;
}

int Number::value() const
{
    return value_;
}

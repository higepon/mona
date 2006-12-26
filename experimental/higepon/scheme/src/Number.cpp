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
    snprintf(buf_, 64, "%d", value_);
    printf("[%s]", buf_);fflush(stdout);
    return std::string(buf_);
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

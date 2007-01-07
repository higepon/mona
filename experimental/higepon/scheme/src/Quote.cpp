#include "Quote.h"

using namespace monash;

Quote::Quote(const std::string& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

Quote::~Quote()
{
}

std::string Quote::toString()
{
    return "quote: \'" + value_;
}

std::string Quote::toStringValue()
{
    return "\'" + value_;
}

int Quote::type() const
{
    return Object::QUOTE;
}

Object* Quote::eval(Environment* env)
{
    return this; // different from SICP's
}

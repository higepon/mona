#include "Quote.h"

using namespace monash;

Quote::Quote(const std::string& value) : value_(value)
{
}

Quote::~Quote()
{
}

std::string Quote::toString()
{
    return "\"" + value_ + "\"";
}

int Quote::type() const
{
    return Object::QUOTE;
}

#include "String.h"

using namespace monash;
using namespace std;

String::String(const string& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

String::~String()
{
}

std::string String::toString()
{
    return "string: \"" + value_ + "\"";
}

std::string String::toStringValue()
{
    return value_;
}

int String::type() const
{
    return Object::STRING;
}

Object* String::eval(Environment* env)
{
    return this;
}

bool String::eqv(Object* o)
{
    if (!o->isString()) return false;
    String* s = (String*)o;
    return this == s;
}

bool String::eq(Object* o)
{
    return false;
}

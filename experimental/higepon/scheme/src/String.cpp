#include "String.h"

using namespace monash;
using namespace std;

String::String(const string& value) : value_(value)
{
}

String::~String()
{
}

std::string String::toString()
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

#include "String.h"

using namespace monash;
using namespace std;

String::String(const string& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

String::String(uint32_t length, uint32_t lineno /* = 0 */) : lineno_(lineno)
{
    // todo ugly
    char* buf = new char[length + 1];
    for (uint32_t i = 0; i < length; i++)
    {
        buf[i] = ' ';
    }
    buf[length] = '\0';
    value_ = string(buf);
}

String::String(uint32_t length, char c, uint32_t lineno /* = 0 */) : lineno_(lineno)
{
    // todo ugly
    char* buf = new char[length + 1];
    for (uint32_t i = 0; i < length; i++)
    {
        buf[i] = c;
    }
    buf[length] = '\0';
    value_ = string(buf);
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

bool String::equal(Object* o)
{
    if (!o->isString()) return false;
    String* s = (String*)o;
    return value() == s->value();
}

Charcter* String::get(uint32_t index)
{
    if (index > value_.size()  || index < 0) return NULL;
    // ugly fix me
    return new Charcter(string("#\\") + value_[index], lineno());
}

bool String::set(uint32_t index, Charcter* c)
{
    if (index > value_.size() - 1 || index < 0) return false;
    value_[index] = c->value();
    return true;
}

#include "String.h"

using namespace monash;
using namespace monash::util;

String::String(const ::util::String& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

String::String(uint32_t length, uint32_t lineno /* = 0 */) : lineno_(lineno)
{
    // todo ugly
    char* buf = new char[length + 1];
    for (int i = 0; i < length; i++)
    {
        buf[i] = ' ';
    }
    buf[length] = '\0';
    value_ = ::util::String(buf);
}

String::String(uint32_t length, char c, uint32_t lineno /* = 0 */) : lineno_(lineno)
{
    // todo ugly
    char* buf = new char[length + 1];
    for (int i = 0; i < length; i++)
    {
        buf[i] = c;
    }
    buf[length] = '\0';
    value_ = ::util::String(buf);
}

String::~String()
{
}

::util::String String::toString()
{
    return "string: \"" + value_ + "\"";
}

::util::String String::toStringValue()
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

Charcter* String::get(int index)
{
    if (index > value_.size()  || index < 0) return NULL;
    // ugly fix me
    return new Charcter(::util::String("#\\") + value_[index], lineno());
}

bool String::set(int index, Charcter* c)
{
    if (index > value_.size() - 1 || index < 0) return false;
    value_[index] = c->value();
    return true;
}

String* String::clone()
{
    return new String(value(), lineno());
}

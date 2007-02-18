#include "SString.h"

using namespace util;
using namespace monash;

SString::SString(const ::util::String& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

SString::SString(uint32_t length, uint32_t lineno /* = 0 */) : lineno_(lineno)
{
    // todo ugly
    char* buf = new char[length + 1];
    for (uint32_t i = 0; i < length; i++)
    {
        buf[i] = ' ';
    }
    buf[length] = '\0';
    value_ = ::util::String(buf);
}

SString::SString(uint32_t length, char c, uint32_t lineno /* = 0 */) : lineno_(lineno)
{
    // todo ugly
    char* buf = new char[length + 1];
    for (uint32_t i = 0; i < length; i++)
    {
        buf[i] = c;
    }
    buf[length] = '\0';
    value_ = ::util::String(buf);
}

SString::~SString()
{
}

String SString::toString()
{
    return "string: \"" + value_ + "\"";
}

String SString::toStringValue()
{
    return value_;
}

int SString::type() const
{
    return Object::STRING;
}

Object* SString::eval(Environment* env)
{
    return this;
}

bool SString::eqv(Object* o)
{
    if (!o->isSString()) return false;
    SString* s = (SString*)o;
    return this == s;
}

bool SString::eq(Object* o)
{
    return false;
}

bool SString::equal(Object* o)
{
    if (!o->isSString()) return false;
    SString* s = (SString*)o;
    return value() == s->value();
}

Charcter* SString::get(uint32_t index)
{
    if (index > value_.size()  || index < 0) return NULL;
    // ugly fix me
    String s = "#\\";
    s += value_[index];
    return new Charcter(s , lineno());
}

bool SString::set(uint32_t index, Charcter* c)
{
    if (index > value_.size() - 1 || index < 0) return false;
    value_.set(index, c->value());
    return true;
}

SString* SString::clone()
{
    return new SString(value(), lineno());
}

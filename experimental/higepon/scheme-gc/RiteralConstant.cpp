#include "RiteralConstant.h"

using namespace util;
using namespace monash;

RiteralConstant::RiteralConstant(const String& value, uint32_t lineno /* = 0 */) : value_(value), lineno_(lineno)
{
}

RiteralConstant::~RiteralConstant()
{
}

String RiteralConstant::toString()
{
    return text();
}

String RiteralConstant::toStringValue()
{
    return text();
}

int RiteralConstant::type() const
{
    return Object::RITERAL_CONSTANT;
}

Object* RiteralConstant::eval(Environment* env)
{
    return this;
}

bool RiteralConstant::eqv(Object* o)
{
    if (!o->isRiteralConstant()) return false;
    RiteralConstant* r = (RiteralConstant*)o;
    return this->toStringValue() == r->toStringValue();
}

bool RiteralConstant::eq(Object* o)
{
    return eqv(o);
}

#include "SRegMatch.h"

using namespace util;
using namespace monash;

SRegMatch::SRegMatch(OnigRegion* region, uint32_t lineno /* = 0 */) : region_(region), lineno_(lineno)
{
}

SRegMatch::~SRegMatch()
{
}

String SRegMatch::toString()
{
    return "#<regmatch>";
}

String SRegMatch::toStringValue()
{
    return toString();
}

int SRegMatch::type() const
{
    return Object::REGMATCH;
}

Object* SRegMatch::eval(Environment* env)
{
    return this;
}

bool SRegMatch::eqv(Object* o)
{
    if (!o->isSRegMatch()) return false;
    SRegMatch* s = (SRegMatch*)o;
    return this == s;
}

bool SRegMatch::eq(Object* o)
{
    return false;
}

bool SRegMatch::equal(Object* o)
{
    return false;
}

int SRegMatch::matchStart()
{
    return 0;
}

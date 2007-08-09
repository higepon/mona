/*!
    \file   SRegexp.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "SRegexp.h"

using namespace util;
using namespace monash;

SRegexp::SRegexp(const ::util::String& pattern, bool caseFold /* = false */, uint32_t lineno /* = 0 */) : pattern_(pattern), caseFold_(caseFold), lineno_(lineno)
{
    int r = onig_new(&reg_,
                     (const uint8_t*)pattern.data(),
                     (const uint8_t*)(pattern.data() + pattern.size()),
                     (ONIG_OPTION_DEFAULT | caseFold ? ONIG_OPTION_IGNORECASE : 0),
                     ONIG_ENCODING_ASCII,
                     ONIG_SYNTAX_RUBY,
                     &einfo_);
    if (r != ONIG_NORMAL)
    {
        char s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str((uint8_t*)s, r, &einfo_);
        fprintf(stderr, "ERROR: %s\n", s);
    }
}

SRegexp::~SRegexp()
{
}

String SRegexp::toString()
{
    return "#" + pattern_ + (caseFold_ ? "i" : "");
}

String SRegexp::toStringValue()
{
    return pattern_;
}

int SRegexp::type() const
{
    return Object::REGEXP;
}

Object* SRegexp::eval(Environment* env)
{
    return this;
}

bool SRegexp::eqv(Object* o)
{
    if (!o->isSRegexp()) return false;
    SRegexp* s = (SRegexp*)o;
    return this == s;
}

bool SRegexp::eq(Object* o)
{
    return false;
}

bool SRegexp::equal(Object* o)
{
    if (!o->isSRegexp()) return false;
    SRegexp* s = (SRegexp*)o;
    return pattern() == s->pattern();
}

Object* SRegexp::apply(Objects* arguments, Environment* env)
{
    ARGC_SHOULD_BE(1);
    SString* s;
    // mona-object?
    Object* isMonaObject;
    SCM_APPLY_PROC("mona.object.object?", as, env, isMonaObject);
    if (isMonaObject->isTrue())
    {
        CAST((new Variable("mona.object.name"))->eval(env), Procedure, p);
        CAST(p->apply(arguments, env), SString, text);
        s = text;
    }
    else
    {
        CAST(ARGV(0), SString, text);
        s = text;
    }

    OnigRegion* region = onig_region_new();
    const uint8_t* start = (const uint8_t*)s->value().data();
    const uint8_t* end   = start + s->value().size();
    const uint8_t* range = end;
    int r = onig_search(reg_, start, end, start, range, region, ONIG_OPTION_NONE);
    if (r >= 0)
    {
        return new SRegMatch(this, region, s->value(), lineno());
    }
    else if (r == ONIG_MISMATCH)
    {
        return SCM_FALSE;
    }
    else
    {
        char s[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str((uint8_t*)s, r);
        RAISE_ERROR(lineno(), "%s", s);
        return NULL;
    }
}


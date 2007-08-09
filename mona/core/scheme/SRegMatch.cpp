/*!
    \file   SRegMatch.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "SRegMatch.h"

using namespace util;
using namespace monash;

SRegMatch::SRegMatch(SRegexp* reg, OnigRegion* region, const String& text, uint32_t lineno /* = 0 */) : reg_(reg), region_(region), text_(text), lineno_(lineno)
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

int SRegMatch::getNameIndex(const ::util::String& name)
{
    return onig_name_to_backref_number(reg_->getReg(), (const uint8_t*)name.data(), (const uint8_t*)(name.data() + name.size()), region_);
}

String SRegMatch::matchAfter(int* result, int i /* = 0 */)
{
    if (i < 0 || i >= region_->num_regs)
    {
        *result = -1;
        return "";
    }
    return text_.substring(region_->end[i], text_.size() - region_->end[i]);
}

String SRegMatch::matchBefore(int* result, int i /* = 0 */)
{
    if (i < 0 || i >= region_->num_regs)
    {
        *result = -1;
        return "";
    }
    return text_.substring(0, region_->beg[i]);
}


int SRegMatch::matchStart(int i /* = 0 */)
{
    if (i < 0 || i >= region_->num_regs)
    {
        return -1;
    }
    return region_->beg[i];
}

int SRegMatch::matchStart(const ::util::String& name)
{
    int i = getNameIndex(name);
    if (i < 0 || i >= region_->num_regs)
    {
        return -1;
    }
    return region_->beg[i];
}

int SRegMatch::matchEnd(int i /* = 0 */)
{
    if (i < 0 || i >= region_->num_regs)
    {
        return -1;
    }
    return region_->end[i];
}

int SRegMatch::matchEnd(const ::util::String& name)
{
    int i = getNameIndex(name);
    if (i < 0 || i >= region_->num_regs)
    {
        return -1;
    }
    return region_->end[i];
}

::util::String SRegMatch::matchSubString(int* result, int i/* = 0 */)
{
    if (i < 0 || i > region_->num_regs)
    {
        *result = -1;
        return "";
    }
    return text_.substring(region_->beg[i], region_->end[i] - region_->beg[i]);
}

::util::String SRegMatch::matchSubString(int* result, const ::util::String& name)
{
    int i = getNameIndex(name);
    if (i < 0 || i >= region_->num_regs)
    {
        *result = -1;
        return "";
    }
    return text_.substring(region_->beg[i], region_->end[i] - region_->beg[i]);
}


Object* SRegMatch::apply(Objects* arguments, Environment* env)
{
    ARGC_SHOULD_BE_BETWEEN(0, 2);
    if (ARGC >= 1 && ARGV(0)->isRiteralConstant())
    {
        RiteralConstant* c = (RiteralConstant*)ARGV(0);
        bool isAfter  = c->text() == "after";
        bool isBefore = c->text() == "before";
        if (isAfter || isBefore)
        {
            Objects* os = new Objects;
            os->add(this);
            for (int i = 1; i < arguments->size(); i ++)
            {
                os->add(arguments->get(i));
            }
            Object* proc = scheme_eval_string(isAfter? "rxmatch-after" : "rxmatch-before", env);
            return Kernel::apply(proc, os, env);
        }
    }

    Objects* os = new Objects;
    os->add(this);
    for (int i = 0; i < arguments->size(); i ++)
    {
        os->add(arguments->get(i));
    }
    Object* proc = scheme_eval_string("rxmatch-substring", env);
    return Kernel::apply(proc, os, env);
}


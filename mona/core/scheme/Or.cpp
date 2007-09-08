/*!
    \file   Or.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/
#include "Or.h"

using namespace util;
using namespace monash;

Or::Or(Cons* cons, uint32_t lineno) : cons_(cons)
{
}

Or::~Or()
{
}

String Or::toString()
{
    return "#<syntax or>";
}

int Or::type() const
{
    return Object::OR;
}

Object* Or::eval(Environment* env)
{
    Object* cdr = cons_->getCdr();
    if (cdr->isNil())
    {
        return SCM_FALSE;
    }
    else if (cdr->isCons())
    {
        FOREACH_LIST(cdr, kar, kdr, "or")
        {
            Object* o = Kernel::eval(kar, env);
            if (!o->isFalse())
            {
                return o;
            }
            else if (kdr->isNil())
            {
                return SCM_FALSE;
            }
        }
    }
    else
    {
        RAISE_ERROR(cons_->lineno(), "syntax-error: malformed or");
    }
    RAISE_ERROR(cons_->lineno(), "syntax-error: malformed or");
    return NULL;
}

bool Or::eqv() const
{
    return false;
}

bool Or::eq() const
{
    return false;
}


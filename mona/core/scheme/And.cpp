/*!
    \file   And.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/08/14 update:$Date$
*/
#include "And.h"

using namespace util;
using namespace monash;

And::And(Cons* cons, uint32_t lineno) : cons_(cons)
{
}

And::~And()
{
}

String And::toString()
{
    return "#<syntax and>";
}

int And::type() const
{
    return Object::AND;
}

Object* And::eval(Environment* env)
{
    Object* cdr = cons_->getCdr();
    if (cdr->isNil())
    {
        return SCM_TRUE;
    }
    else if (cdr->isCons())
    {
        FOREACH_LIST(cdr, kar, kdr, "and")
        {
            Object* o = Kernel::eval(kar, env);
            if (o->isFalse())
            {
                return SCM_FALSE;
            }
            else if (kdr->isNil())
            {
                return o;
            }
        }
    }
    else
    {
        RAISE_ERROR(cons_->lineno(), "syntax-error: malformed and");
    }
    RAISE_ERROR(cons_->lineno(), "syntax-error: malformed and");
    return NULL;
}

bool And::eqv() const
{
    return false;
}

bool And::eq() const
{
    return false;
}


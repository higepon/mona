/*!
    \file   SIf.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "SIf.h"

using namespace util;
using namespace monash;

SIf::SIf(Object* predicate, Object* consequent, Object* alternative, uint32_t lineno) : predicate_(predicate)
                                                                                                  , consequent_(consequent)
                                                                                                  , alternative_(alternative)
                                                                                                  , lineno_(lineno)
{
}

SIf::~SIf()
{
}

String SIf::toString()
{
    return "#<syntax if>";
}

int SIf::type() const
{
    return Object::IF;
}

Object* SIf::eval(Environment* env)
{
    if (!Kernel::evalTailOpt(predicate(), env)->isFalse())
    {
        consequent_->needEval = true;
        consequent_->env      = env;
        return consequent_;
    }
    else
    {
        if (this->alternative() != NULL)
        {
            alternative_->needEval = true;
            alternative_->env      = env;
            return alternative_;
        }
        else
        {
            return SCM_FALSE;
        }
    }
}

bool SIf::eqv() const
{
    return false;
}

bool SIf::eq() const
{
    return false;
}


/*!
    \file   Begin.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "scheme.h"
using namespace util;
using namespace monash;

Begin::Begin(Objects* actions, uint32_t lineno) : actions_(actions), lineno_(lineno)
{
}

Begin::~Begin()
{
}

String Begin::toString()
{
    String ret = "begin:";
    for (int i = 0; i < actions_->size(); i++)
    {
        ret += "[" + actions_->get(i)->toString() + "]";
    }
    return ret;
}

Object* Begin::eval(Environment* env)
{
    return Kernel::evalSequence(this->actions(), env);
}


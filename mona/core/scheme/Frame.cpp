/*!
    \file   Frame.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Frame.h"

using namespace util;
using namespace monash;

Frame::Frame()
{
}

Frame::Frame(Variables* variables, Objects* values)
{
    int size = variables->size() > values->size() ? variables->size() : values->size();
    for (int i = 0; i < size; i++)
    {
        map_.put(variables->get(i)->name().data(), values->get(i));
    }
}

Frame::~Frame()
{
}

Object* Frame::lookup(Variable* variable)
{
    return map_.get(variable->name().data());
}

void Frame::insert(Variable* variable, Object* value)
{
    Object* found = map_.get(variable->name().data());
    if (found != NULL)
    {
        map_.remove(variable->name().data());
    }
    map_.put(variable->name().data(), value);
}

void Frame::remove(Variable* variable)
{
    map_.remove(variable->name().data());
}

String Frame::toString()
{
    String result = "";
    return result;
}

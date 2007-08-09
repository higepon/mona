/*!
    \file   OutputPort.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "OutputPort.h"
#include "scheme.h"

using namespace util;
using namespace monash;

OutputPort::OutputPort(FILE* stream, uint32_t lineno) : stream_(stream), lineno_(lineno)
{
}

OutputPort::~OutputPort()
{
}

String OutputPort::toString()
{
    return "#<output-port>";
}

int OutputPort::type() const
{
    return Object::OUTPUT_PORT;
}

Object* OutputPort::eval(Environment* env)
{
    return this;
}

bool OutputPort::eqv(Object* o)
{
    if (!o->isOutputPort()) return false;
    OutputPort* p = (OutputPort*)o;
    return this == p;
}

bool OutputPort::eq(Object* o)
{
    return eqv(o);
}

bool OutputPort::writeCharacter(Charcter* c)
{
    fputc(c->value(), stream_);
    fflush(stream_);
    return true;
}

bool OutputPort::write(Object* o)
{
    SCHEME_WRITE(stream_, "%s", o->toString().data());
    return true;
}

bool OutputPort::display(Object* o)
{
    SCHEME_WRITE(stream_, "%s", o->toStringValue().data());
    return true;
}

void OutputPort::close()
{
    fclose(stream_);
}

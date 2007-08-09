/*!
    \file   InputPort.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "InputPort.h"

using namespace util;
using namespace monash;

InputPort::InputPort(const String& fileName, FILE* stream, uint32_t lineno) : fileName_(fileName), stream_(stream), lineno_(lineno), fileLineNo_(1)
{
}

InputPort::~InputPort()
{
}

String InputPort::toString()
{
    return "#<input-port>";
}

int InputPort::type() const
{
    return Object::INPUT_PORT;
}

Object* InputPort::eval(Environment* env)
{
    return this;
}

bool InputPort::eqv(Object* o)
{
    if (!o->isInputPort()) return false;
    InputPort* p = (InputPort*)o;
    return this == p;
}

bool InputPort::eq(Object* o)
{
    return eqv(o);
}

Object* InputPort::readCharacter()
{
    char c = fgetc(stream_);
    if (c == '\n') fileLineNo_++;
    if (c == EOF)
    {
        return SCM_EOF;
    }
    else
    {
#ifdef USE_MONA_GC
        return new(false) Charcter(c);
#else
        return new Charcter(c);
#endif
    }
}

char InputPort::readChar()
{
    char c;
    if (buffer_.isEmpty())
    {
        c = fgetc(stream_);
    }
    else
    {
        c = buffer_.removeAt(buffer_.size() - 1);
    }

    if (c == '\n') fileLineNo_++;
    return c;
}

void InputPort::unReadChar(char c)
{
    if (c == '\n') fileLineNo_--;
    buffer_.add(c);
}


Object* InputPort::peekCharacter()
{
    char c = readChar();
    if (c == EOF)
    {
        return SCM_EOF;
    }
    Charcter* ret = new Charcter(c);
    unReadChar(c);
    return ret;
}

void InputPort::close()
{
    fclose(stream_);
}

bool InputPort::charReady()
{
#ifdef MONA
    return false;
#else
    fd_set rfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&rfds);
    FD_SET(fileno(stream_), &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    retval = select(fileno(stream_) + 1, &rfds, NULL, NULL, &tv);
    if (-1 == retval) return false;
    return retval != 0;
#endif
}

::util::String InputPort::getFileName()
{
    return fileName_;
}

int InputPort::getLineNo()
{
    return fileLineNo_;
}

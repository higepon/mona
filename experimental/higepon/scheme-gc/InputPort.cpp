#include "InputPort.h"

using namespace util;
using namespace monash;

InputPort::InputPort(FILE* stream, uint32_t lineno) : stream_(stream), lineno_(lineno)
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
    return fgetc(stream_);
}

void InputPort::unReadChar(char c)
{
    ungetc(c, stream_);
}


Object* InputPort::peekCharacter()
{
    char c = fgetc(stream_);
    if (c == EOF)
    {
        return SCM_EOF;
    }
    Charcter* ret = new Charcter(c);
    ungetc(c, stream_);
    return ret;
}

void InputPort::close()
{
    fclose(stream_);
}

bool InputPort::charReady()
{
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
}

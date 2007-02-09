#include "InputPort.h"

using namespace monash;
using namespace std;

InputPort::InputPort(FILE* stream, uint32_t lineno) : stream_(stream), lineno_(lineno)
{
}

InputPort::~InputPort()
{
}

std::string InputPort::toString()
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

Charcter* InputPort::readCharacter()
{
    return new Charcter(fgetc(stream_));
}

Charcter* InputPort::peekCharacter()
{
    char c = fgetc(stream_);
    Charcter* ret = new Charcter(c);
    ungetc(c, stream_);
    return ret;
}

void InputPort::close()
{
    fclose(stream_);
}

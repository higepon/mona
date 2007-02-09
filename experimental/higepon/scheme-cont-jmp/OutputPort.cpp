#include "OutputPort.h"

using namespace monash;
using namespace std;

OutputPort::OutputPort(FILE* stream, uint32_t lineno) : stream_(stream), lineno_(lineno)
{
}

OutputPort::~OutputPort()
{
}

std::string OutputPort::toString()
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
    fprintf(stream_, "%s", o->toString().c_str());
    return true;
}

bool OutputPort::display(Object* o)
{
    fprintf(stream_, "%s", o->toStringValue().c_str());
    return true;
}

void OutputPort::close()
{
    fclose(stream_);
}

#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

extern OutputPort* g_currentOutputPort;
extern InputPort* g_currentInputPort;

PROCEDURE(EOFObjectP, "eof-object?")
{
    ARGC_SHOULD_BE(1);
    CAST_RETURN_FALSE(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->value() == EOF);
}

PROCEDURE(CloseInputPort, "close-input-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), InputPort, p);
    p->close();
    return new Undef();
}

PROCEDURE(CloseOutputPort, "close-output-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), OutputPort, p);
    p->close();
    return new Undef();
}

PROCEDURE(OpenOutputPort, "open-output-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), String, s);
    FILE* stream = fopen(s->value().c_str(), "w+");
    if (NULL == stream)
    {
        RAISE_ERROR(s->lineno(), "couldn't open output file: %s", s->value().c_str());
    }
    return new OutputPort(stream, lineno());
}

PROCEDURE(OpenInputPort, "open-input-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), String, s);
    FILE* stream = fopen(s->value().c_str(), "r");
    if (NULL == stream)
    {
        RAISE_ERROR(s->lineno(), "couldn't open input file: %s", s->value().c_str());
    }
    return new InputPort(stream, lineno());
}

PROCEDURE(CurrentOutputPort, "current-output-port")
{
    ARGC_SHOULD_BE(0);
    return g_currentOutputPort;
}

PROCEDURE(CurrentInputPort, "current-input-port")
{
    ARGC_SHOULD_BE(0);
    return g_currentInputPort;
}

PROCEDURE(WriteChar, "write-char")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    OutputPort* port;
    if (ARGC == 1)
    {
        port = g_currentOutputPort;
    }
    else
    {
        CAST(ARGV(1), OutputPort, p);
        port = p;
    }
    CAST(ARGV(0), Charcter, c);
    port->writeCharacter(c);
    return new Undef();
}

PROCEDURE(ReadChar, "read-char")
{
    ARGC_SHOULD_BE_BETWEEN(0, 1);
    InputPort* port;
    if (ARGC == 0)
    {
        port = g_currentInputPort;
    }
    else
    {
        printf("%s", ARGV(0)->toString().c_str());
        CAST(ARGV(0), InputPort, p);
        port = p;
    }
    return port->readCharacter();
}

PROCEDURE(PeekChar, "peek-char")
{
    ARGC_SHOULD_BE_BETWEEN(0, 1);
    InputPort* port;
    if (ARGC == 0)
    {
        port = g_currentInputPort;
    }
    else
    {
        CAST(ARGV(0), InputPort, p);
        port = p;
    }
    return port->peekCharacter();
}

PROCEDURE(Write, "write")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    OutputPort* port;
    if (ARGC == 1)
    {
        port = g_currentOutputPort;
    }
    else
    {
        CAST(ARGV(1), OutputPort, p);
        port = p;
    }
    port->write(ARGV(0));
    return new Undef();
}

PROCEDURE(NewLine, "newline")
{
    ARGC_SHOULD_BE_BETWEEN(0, 1);
    OutputPort* port;
    if (ARGC == 0)
    {
        port = g_currentOutputPort;
    }
    else
    {
        CAST(ARGV(0), OutputPort, p);
        port = p;
    }
    port->display(new Charcter('\n'));
    return new Undef();
}

PROCEDURE(Display, "display")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    OutputPort* port;
    if (ARGC == 1)
    {
        port = g_currentOutputPort;
    }
    else
    {
        CAST(ARGV(1), OutputPort, p);
        port = p;
    }
    port->display(ARGV(0));
    return new Undef();
}

PROCEDURE(Load, "load")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), String, s);
    string path  = s->value();
    string input = load(path.c_str());
    if (input == "")
    {
        RAISE_ERROR(s->lineno(), "load error", path.c_str());
        return NULL;
    }
    input = "( " + input + ")";
    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);

    SExp* sexp = SExp::fromString(input);
    SExps sexps = sexp->sexps;
//    Eval* e   = NULL;
    Object* o = NULL;
    for (SExps::iterator p = sexps.begin(); p != sexps.end(); ++p)
    {
        SExp* sex = (*p);
        Quote* quote = new Quote(sex, s->lineno());
        Objects* args = new Objects;
        args->push_back(quote);
        args->push_back(env);
        o = Kernel::apply((new Variable("eval"))->eval(env), args, env, NULL, NULL);
// Object* Kernel::apply(Object* procedure, Objects* arguments, Environment* env, Object* parent, Object* application)
//         e = new Eval(env->translator(), quote, quote->lineno());

//         // let's eval!
//         o =  e->eval(env);
    }
    return o;
}

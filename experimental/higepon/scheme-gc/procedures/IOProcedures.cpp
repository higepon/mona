#include "procedures/Procedure.h"
#include "primitive_procedures.h"
using namespace util;
using namespace std;
using namespace monash;

extern OutputPort* g_currentOutputPort;
extern OutputPort* g_defaultOutputPort;
extern InputPort*  g_defaultInputPort;
extern InputPort*  g_currentInputPort;

PROCEDURE(TranscriptOn, "transcript-on")
{
    ARGC_SHOULD_BE(1);
    if (g_transcript != NULL)
    {
        RAISE_ERROR(lineno(), "already transcipt-on");
    }
    CAST(ARGV(0), SString, s);
    g_transcript = fopen(s->value().data(), "w+");
    if (NULL == g_transcript)
    {
        RAISE_ERROR(s->lineno(), "couldn't open output file: %s", s->value().data());
    }
    return SCM_UNDEF;
}


PROCEDURE(TranscriptOff, "transcript-off")
{
    ARGC_SHOULD_BE(0);
    if (g_transcript != NULL)
    {
        fclose(g_transcript);
        g_transcript = NULL;
    }
    return SCM_UNDEF;
}

PROCEDURE(CharReadyP, "char-ready?")
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
    RETURN_BOOLEAN(port->charReady());
}

PROCEDURE(SetCurrentOutputPort, "set-current-output-port!")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), OutputPort, p);
    g_currentOutputPort = p;
    return SCM_UNDEF;
}

PROCEDURE(SetCurrentInputPort, "set-current-input-port!")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), InputPort, p);
    g_currentInputPort = p;
    return SCM_UNDEF;
}

PROCEDURE(InputPortP, "input-port?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isInputPort());
}

PROCEDURE(OutputPortP, "output-port?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isOutputPort());
}

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
    return SCM_UNDEF;
}

PROCEDURE(CloseOutputPort, "close-output-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), OutputPort, p);
    p->close();
    return SCM_UNDEF;
}

PROCEDURE(OpenOutputPort, "open-output-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), SString, s);
    FILE* stream = fopen(s->value().data(), "w+");
    if (NULL == stream)
    {
        RAISE_ERROR(s->lineno(), "couldn't open output file: %s", s->value().data());
    }
    return new OutputPort(stream, lineno());
}

PROCEDURE(OpenInputPort, "open-input-port")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), SString, s);
    FILE* stream = fopen(s->value().data(), "r");
    if (NULL == stream)
    {
        RAISE_ERROR(s->lineno(), "couldn't open input file: %s", s->value().data());
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
    return SCM_UNDEF;
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
    return SCM_UNDEF;
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
#ifdef USE_MONA_GC
    port->display(new(false) Charcter('\n'));
#else
    port->display(new Charcter('\n'));
#endif
    return SCM_UNDEF;
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
    return SCM_UNDEF;
}

PROCEDURE(Load, "load")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), SString, s);
    String path  = s->value();
    String input = load(path.data());
    if (input == "")
    {
        RAISE_ERROR(s->lineno(), "load error", path.data());
        return NULL;
    }
    input = "( " + input + ")";
    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);
    SExp* sexp = SExp::fromString(input);
    SExps sexps = sexp->sexps;
//    Eval* e   = NULL;
    Object* o = NULL;
    for (int i = 0; i < sexps.size(); i++)
    {

        SExp* sex = sexps[i];
        Quote* quote = new Quote(sex, s->lineno());
        Objects* args = new Objects;
        args->add(quote);
        args->add(env);
        o = Kernel::apply((new Variable("eval"))->eval(env), args, env);
// Object* Kernel::apply(Object* procedure, Objects* arguments, Environment* env, Object* parent, Object* application)
//         e = new Eval(env->translator(), quote, quote->lineno());

//         // let's eval!
//         o =  e->eval(env);
    }

    return o;
}

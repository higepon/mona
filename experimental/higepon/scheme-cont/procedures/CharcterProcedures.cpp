#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(CallCC, "call/cc")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Procedure, procedure);


    // 現在の継続を取り出す Object* cont = xxx->getContinuation()
    if (parent->isBegin())
    {
        Begin* b = (Begin*)parent;
        Object* cont = b->getContinuation(this);
        // Procedure を apply する contを引数に
        Objects* s = new Objects;
        s->push_back(cont);
        Object* o = Kernel::apply(procedure, s, env, NULL, NULL);
        return o;
    }
    else if (parent->isLambda())
    {
        Lambda* lambda = (Lambda*)parent;

        // application is called point of call/cc
        Object* continuation = lambda->getContinuation(application, env);
        Objects* arguments = new Objects;
        arguments->push_back(continuation);
        return Kernel::apply(procedure, arguments, env, NULL, NULL);
    }
    RAISE_ERROR(lineno(), "unknown call/cc");
}

PROCEDURE(CharcterP, "char?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isCharcter());
}

PROCEDURE(CharcterEqualP, "char=?")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->eqv(ARGV(1)));
}

PROCEDURE(CharcterGtP, "char>?")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->gt(ARGV(1)));
}

PROCEDURE(CharcterLtP, "char<?")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->lt(ARGV(1)));
}

PROCEDURE(CharcterGeP, "char>=?")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->ge(ARGV(1)));
}

PROCEDURE(CharcterLeP, "char<=?")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Charcter, c);
    RETURN_BOOLEAN(c->le(ARGV(1)));
}

PROCEDURE(CharcterToInteger, "char->integer")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Charcter, c);
    return c->toNumber();
}

PROCEDURE(IntegerToCharcter, "integer->char")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Number, n);
    return Charcter::fromNumber(n);
}

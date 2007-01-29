#include "procedures/Procedure.h"
#include <setjmp.h>

using namespace monash;
using namespace std;

static jmp_buf cont;

PROCEDURE(CallCC, "call/cc")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Procedure, procedure);

    Continuation* continuation = new Continuation;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (0 == setjmp(continuation->env))
    {
        Objects* arguments = new Objects;
        arguments->push_back(continuation);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        return Kernel::apply(procedure, arguments, env, NULL, NULL);
    }
    else
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//        return continuation->callArugument->eval(env);
        return new Number(1);
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

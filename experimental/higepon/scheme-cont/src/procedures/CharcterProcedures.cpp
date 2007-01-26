#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(CallCC, "call/cc")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Procedure, p);


    // 現在の継続を取り出す Object* cont = xxx->getContinuation()

    // Procedure を apply する contを引数に
 
            RETURN_BOOLEAN(ARGV(0)->isCharcter());
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

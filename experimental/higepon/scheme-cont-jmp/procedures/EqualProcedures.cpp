#include "procedures/Procedure.h"

using namespace util;
using namespace monash;
using namespace std;

PROCEDURE(EqvP, "eqv?")
{
    ARGC_SHOULD_BE(2);
    RETURN_BOOLEAN(ARGV(0)->eqv(ARGV(1)));
}

PROCEDURE(EqP, "eq?")
{
    ARGC_SHOULD_BE(2);
    RETURN_BOOLEAN(ARGV(0)->eq(ARGV(1)));
}

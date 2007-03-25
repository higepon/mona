#include "procedures/Procedure.h"
#include "primitive_procedures.h"

using namespace util;
using namespace monash;
using namespace std;

PROCEDURE(EqvP, "eqv?")
{
    ARGC_SHOULD_BE(2);
    printf("%s %s", ARGV(0)->typeString().data(),ARGV(1)->typeString().data());
    RETURN_BOOLEAN(ARGV(0)->eqv(ARGV(1)));
}

PROCEDURE(EqP, "eq?")
{
    ARGC_SHOULD_BE(2);
    RETURN_BOOLEAN(ARGV(0)->eq(ARGV(1)));
}

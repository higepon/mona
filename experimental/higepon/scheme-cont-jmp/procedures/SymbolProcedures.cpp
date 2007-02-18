#include "procedures/Procedure.h"

using namespace util;
using namespace std;
using namespace monash;

PROCEDURE(SymbolP, "symbol?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isQuote());
}


PROCEDURE(SymbolToString, "symbol->string")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Quote, q);
    return new SString(q->toStringValue(), q->lineno());
}


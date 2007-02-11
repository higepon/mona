#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(SymbolP, "symbol?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isQuote());
}


PROCEDURE(SymbolToString, "symbol->string")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Quote, q);
    return new String(q->toStringValue(), q->lineno());
}


#include "procedures/Procedure.h"
#include "primitive_procedures.h"
using namespace util;
using namespace std;
using namespace monash;

PROCEDURE(SymbolP, "symbol?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isRiteralConstant());
}


PROCEDURE(SymbolToString, "symbol->string")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), RiteralConstant, r);
    return new SString(r->toStringValue(), r->lineno());
}


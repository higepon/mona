#ifndef __PRIMITIVE_PROCEDURE_SYMBOL_PROCEDURES_H__
#define __PRIMITIVE_PROCEDURE_SYMBOL_PROCEDURES_H__

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

#endif // __PRIMITIVE_PROCEDURE_SYMBOL_PROCEDURES_H__

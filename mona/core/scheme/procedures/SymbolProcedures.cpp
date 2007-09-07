/*!
    \file   SymbolProcedures.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "procedures/Procedure.h"
#include "primitive_procedures.h"
using namespace util;
using namespace std;
using namespace monash;

PROCEDURE(SymbolP, "symbol?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isIdentifier());
}


PROCEDURE(SymbolToString, "symbol->string")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Identifier, r);
    return new SString(r->toStringValue(), r->lineno());
}


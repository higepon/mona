/*!
    \file   EqualProcedures.cpp
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

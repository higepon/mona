#include "procedures/Procedure.h"
#include "primitive_procedures.h"
using namespace util;
using namespace std;
using namespace monash;

PROCEDURE(RegexpP, "regexp?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isSRegexp());
}

PROCEDURE(RegexpToString, "regexp->string")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), SRegexp, r);
    return new SString(r->pattern(), lineno());
}

PROCEDURE(RxMatch, "rxmatch")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), SRegexp, r);
    CAST(ARGV(1), SString, s);
    Objects* os = new Objects;
    os->add(s);
    return r->apply(os, env);
}

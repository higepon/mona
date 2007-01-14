#include "PrimitiveProcedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(Display, "display")
{
    ARGC_SHOULD_BE(1);
    printf(ARGV(0)->toStringValue().c_str());
    return new True(); // todo hutei
}

PROCEDURE(Load, "load")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), String, s);
    string path  = s->value();
    string input = load(path.c_str());
    if (input == "")
    {
        RAISE_ERROR(s->lineno(), "load error", path.c_str());
        return NULL;
    }
    input = "( " + input + ")";
    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);

    SExp* sexp = SExp::fromString(input);
    SExps sexps = sexp->sexps;
    Eval* e   = NULL;
    Object* o = NULL;
    for (SExps::iterator p = sexps.begin(); p != sexps.end(); ++p)
    {
        SExp* sex = (*p);
        Quote* quote = new Quote(sex, s->lineno());
        e = new Eval(env->translator(), quote, quote->lineno());

        // let's eval!
        o =  e->eval(env);
    }
    return o;
}

#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

Objects* pairToObjects(Pair* pair)
{
    Objects* objects = new Objects;
    Pair* p = pair;
    for (;;)
    {
        Object* car = p->getCar();
        Object* cdr = p->getCdr();
        if (car != NULL)
        {
            objects->push_back(car);
        }
        if (cdr == NULL) break;
        if (!cdr->isPair()) break;
        p = (Pair*)cdr;
    }
    return objects;
}

PROCEDURE(Exit, "exit")
{
    exit(-1);
}


PROCEDURE(Apply, "apply")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(1), Pair, p);
    return Kernel::apply(ARGV(0), pairToObjects(p), env, NULL, NULL);
}

PROCEDURE(Eval, "eval")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Quote, q);
    CAST(ARGV(1), Environment, e);

    Object* o;
    // see Translator::translateQuote
    SExp* n = q->sexp();
    e->macroFilter().filter(n);
    int ret = e->translator().translate(&n, &o);
    if (ret != Translator::SUCCESS)
    {
        RAISE_ERROR(n->lineno, "eval got error [%s]", toString().c_str(), q->toStringValue().c_str());
    }
    return o->eval(e);
}

PROCEDURE(NullEnvironment, "null-environment")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Number, n);
    if (n->value() != 5)
    {
        RAISE_ERROR(lineno(), "%s got wrong version" , toString().c_str());
    }
    MacroFilter f;
    Translator translator;
    return new Environment(f, translator, lineno());
}

PROCEDURE(SchemeReportEnvironment, "scheme-report-environment")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Number, n);
    if (n->value() != 5)
    {
        RAISE_ERROR(lineno(), "%s got wrong version" , toString().c_str());
    }
    return env;
}

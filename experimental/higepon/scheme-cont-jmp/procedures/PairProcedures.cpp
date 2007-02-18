#include "procedures/Procedure.h"

using namespace util;
using namespace std;
using namespace monash;

PROCEDURE(PairP, "pair?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isPair());
}

PROCEDURE(Cons, "cons")
{
    ARGC_SHOULD_BE(2);
    return new Pair(ARGV(0), ARGV(1));
}

PROCEDURE(Car, "car")
{
    ARGC_SHOULD_BE(1);
    if (ARGV(0)->isQuote())
    {
        Quote* quote = (Quote*)ARGV(0);
        Quote* ret = quote->car();
        if (ret == NULL)
        {
            RAISE_ERROR(quote->lineno(), "%s got error on quote %s", procedureName_.data(), quote->toString().data());
        }
        return ret;
    }
    else if (ARGV(0)->isPair())
    {
        Pair* p = (Pair*)ARGV(0);
        return p->getCar();
    }
    RAISE_ERROR(ARGV(0)->lineno(), "car got [%s], but required pair", procedureName_.data(), ARGV(0)->toString().data());
    return NULL;
}

PROCEDURE(Cdr, "cdr")
{
    ARGC_SHOULD_BE(1);
    Object* o = ARGV(0);
    if (o->isQuote())
    {
        Quote* quote = (Quote*)o;
        Quote* ret = quote->cdr();
        if (ret == NULL)
        {
            RAISE_ERROR(o->lineno(), "%s got error on quote %s", procedureName_.data(), o->toString().data());
        }
        return ret;
    }
    else if (o->isPair())
    {
        Pair* p = (Pair*)o;
        return p->getCdr();
    }
    RAISE_ERROR(o->lineno(), "%s got [%s], but required pair", procedureName_.data(), o->toString().data());
    return NULL;
}

PROCEDURE(SetCar, "set-car!")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Pair, p);
    p->setCar(ARGV(1));
    return new Undef();
}

PROCEDURE(SetCdr, "set-cdr!")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Pair, p);
    p->setCdr(ARGV(1));
    return new Undef();
}

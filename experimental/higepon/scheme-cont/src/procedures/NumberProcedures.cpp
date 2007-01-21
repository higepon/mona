#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(NumberP, "number?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isNumber());
}

PROCEDURE(Plus, "+")
{
    Objects* as = Scheme::listOfValues(arguments, env);
    int total = 0;
    for (Objects::const_iterator it = as->begin(); it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->isNumber())
        {
            Number* n = (Number*)o;
            total += n->value();
        }
    }
    return new Number(total);
}

PROCEDURE(Minus, "-")
{
    ARGC_SHOULD_BE_GT(0);
    CAST(ARGV(0), Number, n);
    if (ARGC == 1) return new Number(-1 * n->value());

    int total = n->value();
    for (Objects::const_iterator it = as->begin() + 1; it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->isNumber())
        {
            Number* nn = (Number*)o;
            total -= nn->value();
        }
    }
    return new Number(total);
}

PROCEDURE(NumberToString, "number->string")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Number, n);
    char buf[32];
    snprintf(buf, 32, "%d", n->value());
    return new String(buf, n->lineno());
}

PROCEDURE(NumberEqual, "=")
{
    ARGC_SHOULD_BE_GT(1);
    CAST_RETURN_FALSE(ARGV(0), Number, n0);
    for (Objects::const_iterator p = as->begin() + 1; p != as->end(); ++p)
    {
        CAST_RETURN_FALSE((*p), Number, n);
        if (n->value() != n0->value()) return new False();
    }
    return new True();
}


PROCEDURE(NumberGt, ">")
{
    ARGC_SHOULD_BE_GT(1);
    CAST_RETURN_FALSE(ARGV(0), Number, prev);
    for (Objects::const_iterator p = as->begin() + 1; p != as->end(); ++p)
    {
        CAST_RETURN_FALSE((*p), Number, n);
        if (prev->value() <= n->value()) return new False();
        prev = n;
    }
    return new True();
}

PROCEDURE(NumberLt, "<")
{
    ARGC_SHOULD_BE_GT(1);
    CAST_RETURN_FALSE(ARGV(0), Number, prev);
    for (Objects::const_iterator p = as->begin() + 1; p != as->end(); ++p)
    {
        CAST_RETURN_FALSE((*p), Number, n);
        if (prev->value() >= n->value()) return new False();
        prev = n;
    }
    return new True();
}

PROCEDURE(NumberGe, ">=")
{
    ARGC_SHOULD_BE_GT(1);
    CAST_RETURN_FALSE(ARGV(0), Number, prev);
    for (Objects::const_iterator p = as->begin() + 1; p != as->end(); ++p)
    {
        CAST_RETURN_FALSE((*p), Number, n);
        if (prev->value() < n->value()) return new False();
        prev = n;
    }
    return new True();
}

PROCEDURE(NumberLe, "<=")
{
    ARGC_SHOULD_BE_GT(1);
    CAST_RETURN_FALSE(ARGV(0), Number, prev);
    for (Objects::const_iterator p = as->begin() + 1; p != as->end(); ++p)
    {
        CAST_RETURN_FALSE((*p), Number, n);
        if (prev->value() > n->value()) return new False();
        prev = n;
    }
    return new True();
}

PROCEDURE(Multiply, "*")
{
    Objects* as = Scheme::listOfValues(arguments, env);
    int total = 1;
    for (Objects::const_iterator it = as->begin(); it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->isNumber())
        {
            Number* n = (Number*)o;
            total *= n->value();
        }
    }
    return new Number(total);
}

PROCEDURE(Divide, "/")
{
    ARGC_SHOULD_BE_GT(0);
    CAST(ARGV(0), Number, n);
    int total = n->value();
    for (Objects::const_iterator it = as->begin() + 1; it != as->end(); it++)
    {
        Object* o = (*it);
        if (o->isNumber())
        {
            Number* nn = (Number*)o;
            if (nn->value() == 0)
            {
                RAISE_ERROR(nn->lineno(), "devide by zero")
                return NULL;
            }
            total /= nn->value();
        }
    }
    return new Number(total);
}


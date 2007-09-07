/*!
    \file   RegexpProcedures.cpp
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

PROCEDURE(RegexpP, "regexp?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isSRegexp());
}

PROCEDURE(RxMatchNumMatches, "rxmatch-num-matches")
{
    ARGC_SHOULD_BE(1);
    if (ARGV(0)->isFalse())
    {
        return new Number(0, lineno());
    }

    CAST(ARGV(0), SRegMatch, m);
    return new Number(m->getNumMatches(), lineno());
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

PROCEDURE(RxMatchStart, "rxmatch-start")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGV(0)->isFalse())
    {
        return SCM_FALSE;
    }

    CAST(ARGV(0), SRegMatch, m);
    int ret;
    if (ARGC == 2 && ARGV(1)->isIdentifier())
    {
        Identifier* c = (Identifier*)ARGV(1);
        ret = m->matchStart(c->text());
        if (-1 == ret)
        {
            RAISE_ERROR(ARGV(0)->lineno(), "named submatch not found: %s", c->text().data());
        }
    }
    else
    {
        int i = 0;
        if (ARGC == 2)
        {
            CAST(ARGV(1), Number, n);
            i = n->value();
        }
        ret = m->matchStart(i);
        if (-1 == ret)
        {
            RAISE_ERROR(ARGV(0)->lineno(), "submatch index out of range: %d", i);
        }
    }
    return new Number(ret, lineno());
}

PROCEDURE(RxMatchEnd, "rxmatch-end")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGV(0)->isFalse())
    {
        return SCM_FALSE;
    }

    CAST(ARGV(0), SRegMatch, m);
    int ret;
    if (ARGC == 2 && ARGV(1)->isIdentifier())
    {
        Identifier* c = (Identifier*)ARGV(1);
        ret = m->matchEnd(c->text());
        if (-1 == ret)
        {
            RAISE_ERROR(ARGV(0)->lineno(), "named submatch not found: %s", c->text().data());
        }
    }
    else
    {
        int i = 0;
        if (ARGC == 2)
        {
            CAST(ARGV(1), Number, n);
            i = n->value();
        }
        ret = m->matchEnd(i);
        if (-1 == ret)
        {
            RAISE_ERROR(ARGV(0)->lineno(), "submatch index out of range: %d", i);
        }
    }
    return new Number(ret, lineno());
}

PROCEDURE(RxMatchSubString, "rxmatch-substring")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGV(0)->isFalse())
    {
        return SCM_FALSE;
    }

    CAST(ARGV(0), SRegMatch, m);
    String ret;
    if (ARGC == 2 && ARGV(1)->isIdentifier())
    {
        Identifier* c = (Identifier*)ARGV(1);
        int result;
        ret = m->matchSubString(&result, c->text());
        if (-1 == result)
        {
            RAISE_ERROR(ARGV(0)->lineno(), "named submatch not found: %s", c->text().data());
        }
    }
    else
    {
        int i = 0;
        if (2 == ARGC)
        {
            CAST(ARGV(1), Number, n);
            i = n->value();
        }
        int result;
        ret = m->matchSubString(&result, i);
        if (-1 == result)
        {
            RAISE_ERROR(ARGV(0)->lineno(), "submatch index out of range:%d", i);
        }
    }
    return new SString(ret, lineno());
}

PROCEDURE(RxMatchAfter, "rxmatch-after")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGV(0)->isFalse())
    {
        return SCM_FALSE;
    }

    CAST(ARGV(0), SRegMatch, m);
    String ret;
    int i = 0;
    if (2 == ARGC)
    {
        CAST(ARGV(1), Number, n);
        i = n->value();
    }
    int result;
    ret = m->matchAfter(&result, i);
    if (-1 == result)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "submatch index out of range:%d", i);
    }
    return new SString(ret, lineno());
}

PROCEDURE(RxMatchBefore, "rxmatch-before")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGV(0)->isFalse())
    {
        return SCM_FALSE;
    }

    CAST(ARGV(0), SRegMatch, m);
    String ret;
    int i = 0;
    if (2 == ARGC)
    {
        CAST(ARGV(1), Number, n);
        i = n->value();
    }
    int result;
    ret = m->matchBefore(&result, i);
    if (-1 == result)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "submatch index out of range:%d", i);
    }
    return new SString(ret, lineno());
}

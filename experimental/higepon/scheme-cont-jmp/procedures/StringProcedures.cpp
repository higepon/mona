#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(StringP, "string?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isString());
}

PROCEDURE(MakeString, "make-string")
{
    ARGC_SHOULD_BE_BETWEEN(1, 2);
    if (ARGC == 1)
    {
        CAST(ARGV(0), Number, n)
        return new String(n->value(), n->lineno());
    }
    else
    {
        CAST(ARGV(0), Number, n);
        CAST(ARGV(1), Charcter, c);
        return new String(n->value(), c->value(), n->lineno());
    }
}

PROCEDURE(StringLength, "string-length")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), String, s);
    return new Number(s->value().size(), s->lineno());
}

PROCEDURE(StringRef, "string-ref")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), String, s);
    CAST(ARGV(1), Number, n);
    Object* ret = s->get(n->value());
    if (ret == NULL)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong index", toString().c_str());
        return NULL;
    }
    return ret;
}

PROCEDURE(StringSet, "string-set!")
{
    ARGC_SHOULD_BE(3);
    CAST(ARGV(0), String, s);
    CAST(ARGV(1), Number, n);
    CAST(ARGV(2), Charcter, c);
    bool isOK = s->set(n->value(), c);
    if (!isOK)
    {
        RAISE_ERROR(ARGV(0)->lineno(), "%s got wrong arguments", toString().c_str());
    }
    return new Undef();
}


PROCEDURE(StringEqualP, "string=?")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), String, s);
    RETURN_BOOLEAN(s->equal(ARGV(1)));
}

PROCEDURE(StringAppend, "string-append")
{
    ARGC_SHOULD_BE_GT(0)
    string text = "";

    for (Objects::const_iterator p = arguments->begin(); p != arguments->end(); ++p)
    {
        if ((*p)->type() != Object::STRING) continue;
        String* s = (String*)(*p);
        text += s->value();
    }
    return new String(text);
}

PROCEDURE(StringToNumber, "string->number")
{
    ARGC_SHOULD_BE(1);
    CAST_RETURN_FALSE(ARGV(0), String, str);
    string text = str->value();
    for (std::string::size_type i = 0; i < text.length(); i++)
    {
        if (!isdigit(text[i]))
        {
            return new False();
        }
    }
    return new Number(atoi(text.c_str()));
}

PROCEDURE(StringToSymbol, "string->symbol")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), String, s);
    SExp* exp = new SExp(SExp::SYMBOL);
    exp->text = s->toStringValue();
    return new Quote(exp);
}

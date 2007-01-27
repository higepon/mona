#include "procedures/Procedure.h"

using namespace monash;
using namespace std;

PROCEDURE(StringP, "string?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isString());
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


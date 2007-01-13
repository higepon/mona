#ifndef __PRIMITIVE_PROCEDURE_STRING_PROCEDURES_H__
#define __PRIMITIVE_PROCEDURE_STRING_PROCEDURES_H__

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
    Objects* as = listOfValues(arguments, env);
    if (ARGC == 0)
    {
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0, "string-append got %d arguments, but required at least %d", ARGC, 1);
        return NULL;
    }
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
    if (!ARGV(0)->isString()) return new False();
    String* str = (String*)ARGV(0);
    std::string text = str->value();
    for (std::string::size_type i = 0; i < text.length(); i++)
    {
        if (!isdigit(text[i]))
        {
            return new False();
        }
    }
    return new Number(atoi(text.c_str()));
}

#endif // __PRIMITIVE_PROCEDURE_STRING_PROCEDURES_H__


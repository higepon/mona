#ifndef __PRIMITIVE_PROCEDURE_NUMBER_PROCEDURES_H__
#define __PRIMITIVE_PROCEDURE_NUMBER_PROCEDURES_H__

PROCEDURE(NumberP, "number?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isNumber());
}

PROCEDURE(Plus, "+")
{
    Objects* as = listOfValues(arguments, env);
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
    Objects* as = listOfValues(arguments, env);
    if (ARGC == 0 || !ARGV(0)->isNumber())
    {
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0, "minus got wrong arguments")
        return NULL;
    }
    Number* n = (Number*)ARGV(0);
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

PROCEDURE(NumberEqual, "=")
{
    Objects* as = listOfValues(arguments, env);
    if (ARGC != 2) return new False();
    Object* o1 = ARGV(0);
    Object* o2 = ARGV(1);
    if (!o1->isNumber() || !o2->isNumber()) return new False();
    Number* n1 = (Number*)o1;
    Number* n2 = (Number*)o2;
    RETURN_BOOLEAN(n1->value() == n2->value());
}

PROCEDURE(Multiply, "*")
{
    Objects* as = listOfValues(arguments, env);
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
    Objects* as = listOfValues(arguments, env);
    if (ARGC == 0 || !ARGV(0)->isNumber())
    {
        RAISE_ERROR(ARGC >= 0 ? ARGV(0)->lineno() : 0, "devide got wrong arguments")
        return NULL;
    }
    Number* n = (Number*)ARGV(0);

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

#endif // __PRIMITIVE_PROCEDURE_NUMBER_PROCEDURES_H__


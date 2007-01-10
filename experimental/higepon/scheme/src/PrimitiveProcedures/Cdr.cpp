#include "Cdr.h"

using namespace monash;

Cdr::Cdr()
{
}

Cdr::~Cdr()
{
}

std::string Cdr::toString()
{
    return "procedure:cdr";
}

Object* Cdr::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* Cdr::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 1)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "cdr got %d arguments, but required %d", as->size(), 1);
        return NULL;
    }
    Object* o = as->at(0);
    if (o->type() == Object::QUOTE)
    {
        Quote* quote = (Quote*)o;
        Quote* ret = quote->cdr();
        if (ret == NULL)
        {
            RAISE_ERROR(o->lineno(), "cdr got error on quote %s", o->toString().c_str());
        }
        return ret;
    }
    else if (o->type() == Object::PAIR)
    {
        Pair* p = (Pair*)o;
        return p->second();
    }
    RAISE_ERROR(o->lineno(), "cdr got [%s], but required pair", o->toString().c_str());
    return NULL;
}

#include "CharcterP.h"

using namespace monash;

using namespace std;

CharcterP::CharcterP()
{
}

CharcterP::~CharcterP()
{
}

std::string CharcterP::toString()
{
    return "procedure:charcterp";
}

Object* CharcterP::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* CharcterP::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 0)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "charcterp got %d arguments, but required %d", as->size(), 0);
        return NULL;
    }
    if (as->at(0)->isCharcter())
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

PROCEDURE_BODY(CharcterEqualP, arguments, env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "%s got %d arguments, but required %d", toString().c_str(), as->size(), 2);
        return NULL;
    }
    Object* o = as->at(0);
    if (!o->isCharcter())
    {
        return new False();
    }
    Charcter* c = (Charcter*)o;
    if (c->eqv(as->at(1)))
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

PROCEDURE_BODY(CharcterGtP, arguments, env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "%s got %d arguments, but required %d", toString().c_str(), as->size(), 2);
        return NULL;
    }
    Object* o = as->at(0);
    if (!o->isCharcter())
    {
        return new False();
    }
    Charcter* c = (Charcter*)o;
    if (c->gt(as->at(1)))
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

PROCEDURE_BODY(CharcterLtP, arguments, env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "%s got %d arguments, but required %d", toString().c_str(), as->size(), 2);
        return NULL;
    }
    Object* o = as->at(0);
    if (!o->isCharcter())
    {
        return new False();
    }
    Charcter* c = (Charcter*)o;
    if (c->lt(as->at(1)))
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

PROCEDURE_BODY(CharcterGeP, arguments, env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "%s got %d arguments, but required %d", toString().c_str(), as->size(), 2);
        return NULL;
    }
    Object* o = as->at(0);
    if (!o->isCharcter())
    {
        return new False();
    }
    Charcter* c = (Charcter*)o;
    if (c->ge(as->at(1)))
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

PROCEDURE_BODY(CharcterLeP, arguments, env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 2)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "%s got %d arguments, but required %d", toString().c_str(), as->size(), 2);
        return NULL;
    }
    Object* o = as->at(0);
    if (!o->isCharcter())
    {
        return new False();
    }
    Charcter* c = (Charcter*)o;
    if (c->le(as->at(1)))
    {
        return new True();
    }
    else
    {
        return new False();
    }
}

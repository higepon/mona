#include "Set.h"

using namespace monash;

Set::Set()
{
}

Set::~Set()
{
}

std::string Set::toString()
{
    return "procedure:set!";
}

Object* Set::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* Set::apply(Objects* arguments, Environment* env)
{
    // we do not eval all arguments, because "a" of (set! a 3) need not to be eval.
    //Objects* as = listOfValues(arguments, env);

    if (arguments->size() != 2)
    {
        RAISE_ERROR(arguments->size() > 0 ? arguments->at(0)->lineno() : 0, "set! got %d arguments, but required %d", arguments->size(), 2);
        return NULL;
    }

    if (arguments->at(0)->type() != Object::VARIABLE)
    {
        RAISE_ERROR(arguments->at(0)->lineno(), "set! got [%s] , but required variable", arguments->at(0)->toString().c_str());
        return NULL;
    }

    Variable* v = (Variable*)arguments->at(0);
    env->setVaribale(v, arguments->at(1)->eval(env));
    return new Number(0); // hutei
}

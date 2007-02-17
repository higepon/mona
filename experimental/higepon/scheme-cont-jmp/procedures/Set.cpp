#include "Set.h"

using namespace monash;

Set::Set()
{
}

Set::~Set()
{
}

::util::String Set::toString()
{
    return "procedure:set!";
}

Object* Set::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().data());
    return NULL;
}

Object* Set::apply(Objects* arguments, Environment* env)
{
    // we do not eval all arguments, because "a" of (set! a 3) need not to be eval.
    //Objects* as = listOfValues(arguments, env);

    if (arguments->size() != 2)
    {
        RAISE_ERROR(arguments->size() > 0 ? arguments->get(0)->lineno() : 0, "set! got %d arguments, but required %d", arguments->size(), 2);
        return NULL;
    }

    if (arguments->get(0)->type() != Object::VARIABLE)
    {
        RAISE_ERROR(arguments->get(0)->lineno(), "set! got [%s] , but required variable", arguments->get(0)->toString().data());
        return NULL;
    }

    Variable* v = (Variable*)arguments->get(0);
    Object* value = arguments->get(1)->eval(env);
    env->setVaribale(v, value);
    return value; // hutei
}

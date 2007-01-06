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
    printf("don't eval me");
    return NULL;
}

Object* Set::apply(Objects* arguments, Environment* env)
{
    // we do not eval all arguments, because "a" of (set! a 3) need not to be eval.
    //Objects* as = listOfValues(arguments, env);

    if (arguments->size() != 2)
    {
        fprintf(stderr, "set! wrong arguments\n");
        return NULL;
    }

    if (arguments->at(0)->type() != Object::VARIABLE)
    {
        fprintf(stderr, "set! %d?\n", arguments->at(0)->type());
        return NULL;
    }

    Variable* v = (Variable*)arguments->at(0);
    env->setVaribale(v, arguments->at(1)->eval(env));
    return new Number(0); // hutei
}

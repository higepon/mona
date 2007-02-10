#include "Continuation.h"
using namespace monash;
using namespace std;

Continuation::Continuation(uint32_t lineno /* = 0 */) : lineno_(lineno)
{
}

Continuation::~Continuation()
{
}

std::string Continuation::toString()
{
    return "continuation";
}

std::string Continuation::toStringValue()
{
    return toString();
}

int Continuation::type() const
{
    return Object::CONTINUATION;
}

Object* Continuation::eval(Environment* env)
{
    return this;
}

bool Continuation::eqv(Object* o)
{
    if (!o->isContinuation()) return false;
    Continuation* s = (Continuation*)o;
    return this == s;
}

bool Continuation::eq(Object* o)
{
    return false;
}

bool Continuation::equal(Object* o)
{
    return eqv(o);
}

Object* Continuation::apply(Objects* arguments, Environment* env)
{
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    ARGC_SHOULD_BE(1);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    callArugument = ARGV(0);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    cont_restore(&cont, 1);
    /* NOT REACHED */
    return NULL;
}

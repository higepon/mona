#include "StringAppend.h"

using namespace monash;
using namespace std;

StringAppend::StringAppend()
{
}

StringAppend::~StringAppend()
{
}

std::string StringAppend::toString()
{
    return "procedure:number?";
}

Object* StringAppend::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* StringAppend::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);
    if (as->size() == 0)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "string-append got %d arguments, but required at least %d", as->size(), 1);
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

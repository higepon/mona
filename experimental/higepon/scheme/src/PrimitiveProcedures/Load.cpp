#include "Load.h"

using namespace monash;
using namespace std;

Load::Load()
{
}

Load::~Load()
{
}

std::string Load::toString()
{
    return "procedure:load";
}

Object* Load::eval(Environment* env)
{
    RAISE_ERROR(lineno(), "don't eval procedure [%s]", toString().c_str());
    return NULL;
}

Object* Load::apply(Objects* arguments, Environment* env)
{
    Objects* as = listOfValues(arguments, env);

    if (as->size() != 1)
    {
        RAISE_ERROR(as->size() >= 0 ? as->at(0)->lineno() : 0, "load got %d arguments, but required %d", as->size(), 1);
        return NULL;
    }
    Object* arg = as->at(0);
    if (arg->type() != Object::STRING)
    {
        RAISE_ERROR(arg->lineno(), "load got require string argument");
        return NULL;
    }
    String* s = (String*)arg;
    string path  = s->value();
    string input = load(path.c_str());
    if (input == "")
    {
        RAISE_ERROR(s->lineno(), "load error", path.c_str());
        return NULL;
    }
    input = "( " + input + ")";
    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);

    SExp* sexp = SExp::fromString(input);
    SExps sexps = sexp->sexps;
    Eval* e   = NULL;
    Object* o = NULL;
    for (SExps::iterator p = sexps.begin(); p != sexps.end(); ++p)
    {
        SExp* sex = (*p);
        Quote* quote = new Quote(sex, s->lineno());
        e = new Eval(env->translator(), quote, quote->lineno());

        // let's eval!
        o =  e->eval(env);
    }
    return o;
}


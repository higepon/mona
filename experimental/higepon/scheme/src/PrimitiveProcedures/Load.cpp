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
    Object* o = as->at(0);
    if (o->type() != Object::STRING)
    {
        RAISE_ERROR(o->lineno(), "load got require string argument");
        return NULL;
    }
    String* s = (String*)o;
    string path  = s->value();
    string input = load(path.c_str());
    if (input == "")
    {
        RAISE_ERROR(o->lineno(), "load error", path.c_str());
        return NULL;
    }
    input = "(begin " + input + ")";
    Node* node = Node::fromString(input);
    Quote* quote = new Quote(node, o->lineno());
//    printf("<<%s>>\n", quote->toStringValue().c_str());
    Eval* e = new Eval(env->translator(), quote, quote->lineno());
    return e->eval(env);
}


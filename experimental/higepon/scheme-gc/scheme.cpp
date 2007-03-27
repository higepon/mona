#define GLOBAL_VALUE_DEFINED
#include "scheme.h"

//using namespace util;
using namespace monash;

#include "primitive_procedures.h"

void registerPrimitives(Environment* env)
{
    g_defaultOutputPort = new OutputPort(stdout);
    g_defaultInputPort  = new InputPort(stdin);
    g_currentInputPort  = g_defaultInputPort;
    g_currentOutputPort = g_defaultOutputPort;
    g_transcript = NULL;
    //    g_provide_map = new HashMap<int>;
    g_dynamic_winds = new DynamicWinds;
    g_true = new True;
    g_false = new False;
    g_undef = new Undef;
    g_no_arg = new Objects;
    g_nil = new Nil;

#include "register.inc"


//     for (int i = 0; i < procedures.size(); i++)
//     {
//         env->defineVariable(procedures[i].first, procedures[i].second);
//     }
    env->defineVariable(new Variable("#f"),             g_false);
    env->defineVariable(new Variable("#t"),             g_true);
    env->defineVariable(new Variable("set!"),           new Set());
}

SExp* objectToSExp(Object* o)
{
    SExp* sexp;
    if (o->isNumber())
    {
        sexp = new SExp(SExp::NUMBER, o->lineno());
        Number* n = (Number*)o;
        sexp->value = n->value();
    }
    else if (o->isNil())
    {
        sexp = new SExp(SExp::SEXPS, o->lineno());
//        SExp* quoteSexp = new SExp(SExp::SYMBOL);
        //       quoteSexp->text = "quote";
//        sexp->sexps.add(quoteSexp);
//        sexp->sexps.add(new SExp(SExp::SEXPS));
    }
    else if (o->isCharcter())
    {
        sexp = new SExp(SExp::CHAR, o->lineno());
        Charcter* c = (Charcter*)o;
        sexp->text = c->stringValue();

    }
    else if (o->isSString())
    {
        sexp = new SExp(SExp::STRING, o->lineno());
        SString* s = (SString*)o;
        sexp->text = s->value();
    }
    else if (o->isVariable())
    {
        sexp = new SExp(SExp::SYMBOL, o->lineno());
        Variable* v = (Variable*)o;
        sexp->text = v->name();
    }
    else if (o->isRiteralConstant())
    {
        sexp = new SExp(SExp::SYMBOL, o->lineno());
        RiteralConstant* r = (RiteralConstant*)o;
//        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        sexp->text = r->text();
    }
    else if (o->isPair())
    {
        Pair* p = (Pair*)o;
        sexp = pairToSExp(p);
    }
    else if (o->isTrue())
    {
        sexp = new SExp(SExp::SYMBOL, o->lineno());
        sexp->text = "#t";
    }
    else if (o->isFalse())
    {
        sexp = new SExp(SExp::SYMBOL, o->lineno());
        sexp->text = "#f";
    }
    else
    {
        RAISE_ERROR(o->lineno(), "objectToSExp error %s\n", o->typeString().data());
    }
    return sexp;
}

SExp* pairToSExp(Pair* p)
{
    SExp* sexp = new SExp(SExp::SEXPS);
    Object* o = p;
    for (;;)
    {
        if (o->isNil())
        {
            break;
        }
        else if (o->isPair())
        {
            Pair* p = (Pair*)o;
            sexp->sexps.add(::objectToSExp(p->getCar()));
            o = p->getCdr();
        }
        else
        {
            sexp->sexps.add(::objectToSExp(o));
            break;
        }
    }
//    printf("result=%s\n", sexp->toSExpString().data());
    return sexp;
}

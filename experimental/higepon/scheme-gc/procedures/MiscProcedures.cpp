#include "procedures/Procedure.h"
#include "primitive_procedures.h"
using namespace std;
using namespace monash;

Objects* pairToObjects(Pair* pair)
{
    Objects* objects = new Objects;
#if 1
    Pair* p = pair;
    for (;;)
    {
        Object* car = p->getCar();
        Object* cdr = p->getCdr();
        if (car != NULL)
        {
            objects->add(car);
        }
        if (cdr == NULL) break;
        if (!cdr->isPair()) break;
        p = (Pair*)cdr;
    }
#else
    objects->add(pair);
#endif
    return objects;
}

// PROCEDURE(Provide, "provide")
// {
//     ARGC_SHOULD_BE(1);
//     CAST(ARGV(0), SString, s);
//     g_provide_map->put(s->value().data(), 1);
//     return s;
// }

// PROCEDURE(ProvidedP, "provided?")
// {
//     ARGC_SHOULD_BE(1);
//     CAST(ARGV(0), SString, s);
//     RETURN_BOOLEAN(g_provide_map->get(s->value().data()) != NULL);
// }

PROCEDURE(DynamicWindProc, "dynamic-wind")
{
    ARGC_SHOULD_BE(3);
    DynamicWind* dynamicWind = new DynamicWind(ARGV(0), ARGV(1), ARGV(2));
    g_dynamic_winds->add(dynamicWind);
    Kernel::apply(ARGV(0), SCM_NO_ARG, env);
    Object* ret = Kernel::apply(ARGV(1), SCM_NO_ARG, env);
    Kernel::apply(ARGV(2), SCM_NO_ARG, env);
    g_dynamic_winds->removeAt(0);
    return ret;
}

PROCEDURE(CallWithValues, "call-with-values")
{
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Procedure, producer);
    CAST(ARGV(1), Procedure, consumer);
    Object* applyed = Kernel::apply(producer, new Objects, env);
    Objects* applyeds = new Objects;
    applyeds->add(applyed);
    return Kernel::apply(consumer, applyeds, env);
}

PROCEDURE(CallWithCurrentContinuation, "call-with-current-continuation")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Procedure, procedure);

    Continuation* continuation = new Continuation;
    if (0 == cont_save(&(continuation->cont)))
    {
        int wind_size = g_dynamic_winds->size();
        if (wind_size != 0)
        {
           continuation->dynamicWind = g_dynamic_winds->get(0);
           g_dynamic_winds->removeAt(0);
        }

        Objects* arguments = new Objects;
        arguments->add(continuation);
        return Kernel::apply(procedure, arguments, env);
    }
    else
    {
        int wind_size = g_dynamic_winds->size();
        if (wind_size != 0)
        {
            for (int i = wind_size - 1; i >= 0; i--)
            {
                DynamicWind* d = g_dynamic_winds->get(i);
                Kernel::apply(d->after, SCM_NO_ARG, env);
                g_dynamic_winds->removeAt(i);
            }
        }
        else if (continuation->dynamicWind != NULL)
        {
            Kernel::apply(continuation->dynamicWind->before, SCM_NO_ARG, env);
        }

        if (continuation->callAruguments->size() == 1)
        {
            Object* result = continuation->callAruguments->get(0)->eval(env);
            return result;
        }
        else
        {
            return new Values(continuation->callAruguments, lineno());
        }
    }

    RAISE_ERROR(lineno(), "unknown call/cc");
}

PROCEDURE(NotSupported, "not-supported")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), SString, s);
    RAISE_ERROR(0, "%s not supported\n", s->toStringValue().data());
    RETURN_BOOLEAN(false);
}

PROCEDURE(ProcedureP, "procedure?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isProcedure());
}

PROCEDURE(BooleanP, "boolean?")
{
    ARGC_SHOULD_BE(1);
    RETURN_BOOLEAN(ARGV(0)->isBoolean());
}

#ifdef USE_MONA_GC
extern "C" {
#include "gc/gc.h"
};
#endif
PROCEDURE(Exit, "exit")
{
#ifdef USE_MONA_GC
    gc();
    gc_fini();
#endif
    exit(-1);
}


PROCEDURE(Apply, "apply")
{
    ARGC_SHOULD_BE_GT(1);
    Objects* tmp = new Objects;
    if (ARGC == 2 && ARGV(1)->isNil())
    {
        tmp->add(ARGV(1));
        return Kernel::apply(ARGV(0), tmp, env);
    }
    CAST(ARGV(ARGC - 1), Pair, p);
    Objects* os = pairToObjects(p);

    for(int i = 1; i < ARGC -1; i++)
    {
        tmp->add(ARGV(i));
    }
    for (int i = 0; i < os->size(); i++)
    {
        tmp->add(os->get(i));
    }

    return Kernel::apply(ARGV(0), tmp, env);
}

PROCEDURE(Eval, "eval")
{
#if 0
    ARGC_SHOULD_BE(2);
    CAST(ARGV(0), Quote, q);
    CAST(ARGV(1), Environment, e);

    Object* o;
   // see Translator::translateQuote
    SExp* n = q->sexp();
    e->macroFilter().filter(n);
    int ret = e->translator().translate(&n, &o);
    if (ret != Translator::SUCCESS)
    {
        RAISE_ERROR(n->lineno, "eval got error [%s]", toString().data(), q->toStringValue().data());
    }
    return o->eval(e);
#else
    ARGC_SHOULD_BE(2);
    CAST(ARGV(1), Environment, e);
    Object* target = ARGV(0);

//     if (target->isPair())
//     {
//         Pair* p = (Pair*)target;
        SExp* sexp = objectToSExp(target);//::pairToSExp(p);
        e->macroFilter().filter(sexp);
        Object* o;
        int ret = e->translator().translate(&sexp, &o);
        if (ret != Translator::SUCCESS)
        {
            RAISE_ERROR(sexp->lineno, "eval got error [%s]", toString().data(), target->toStringValue().data());
        }
        return o->eval(e);
//     }
//     else
//     {
//         printf("%s %s:%d %s\n", __func__, __FILE__, __LINE__, target->typeString().data());fflush(stdout);// debug
//         return target->eval(e);
//     }
#endif
}

PROCEDURE(NullEnvironment, "null-environment")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Number, n);
    if (n->value() != 5)
    {
        RAISE_ERROR(lineno(), "%s got wrong version" , toString().data());
    }
    MacroFilter f;
    Translator translator;
    return new Environment(f, translator, lineno());
}

PROCEDURE(SchemeReportEnvironment, "scheme-report-environment")
{
    ARGC_SHOULD_BE(1);
    CAST(ARGV(0), Number, n);
    if (n->value() != 5)
    {
        RAISE_ERROR(lineno(), "%s got wrong version" , toString().data());
    }
    return env;
}

PROCEDURE(InteractionEnvironment, "interaction-environment")
{
    return env;
}

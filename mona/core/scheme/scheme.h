/*!
    \file  scheme.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __SCHEME_H__
#define __SCHEME_H__

#ifdef GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#ifdef MONA
#include <monapi.h>
#include <servers/screen.h>
#else
#include <sys/stat.h>    // stat
#include <sys/time.h>    // rlimit
#include <sys/resource.h> // rlimit
#endif
#include "util/Vector.h"
#include "util/String.h"
#include "util/Pair.h"
#include "util/HashMap.h"

namespace util {
    typedef Vector<String*> Strings;
};

#include "Object.h"
#include "Environment.h"
#include "Procedure.h"
#include "Number.h"
#include "SString.h"
#include "Charcter.h"
#include "Vector.h"
#include "Cons.h"
#include "Set.h"
#include "SIf.h"
#include "And.h"
#include "Or.h"
#include "Begin.h"
#include "Definition.h"
#include "Application.h"
#include "Continuation.h"
#include "Translator.h"
#include "TraditionalMacro.h"
#include "Error.h"
#include "Variant.h"
#include "Parser.h"
#include "OutputPort.h"
#include "InputPort.h"
#include "StringReader.h"
#include "Scanner.h"
#include "Values.h"
#include "Nil.h"
#include "Eof.h"
#include "MonaTerminal.h"
#include "Interaction.h"
#include "Identifier.h"
#include "SRegexp.h"
#include "SRegMatch.h"
#include "True.h"
#include "False.h"
#include "Undef.h"
#include "procedures/Procedure.h"

namespace monash {
class DynamicWind;
class Cons;
class InputPort;
typedef ::util::Vector<DynamicWind*> DynamicWinds;
typedef ::util::Vector< ::util::Pair<Variable*, Object*> > DefaultProcedures;

}

#ifdef MONA
#define SCM_TRACE_OUT(...)  _logprintf(__VA_ARGS__), _logprintf("%s:%d:(%s)\n", __FILE__, __LINE__, __func__) ;
#else
#define SCM_TRACE_OUT(...) /* */
#endif
::util::String load(const ::util::String& file);
void scheme_register_primitives(monash::Environment* env);
void scheme_const_init();
void scheme_expand_stack(uint32_t mb);
void scheme_interactive();
bool scheme_on_input_line(const ::util::String& line);
void scheme_on_reedit();
void scheme_init();
monash::Object* scheme_eval_string(const ::util::String& input, monash::Environment* env, bool out = false);
int scheme_batch(const ::util::String& file);
//nmonash::SExp* objectToSExp(monash::Object* o);
//monash::SExp* pairToSExp(monash::Cons* p);


GLOBAL ::monash::True* g_true;
GLOBAL ::monash::False* g_false;
GLOBAL ::monash::Eof* g_eof;
GLOBAL ::monash::Undef* g_undef;
GLOBAL ::monash::Nil* g_nil;
GLOBAL ::monash::Cons* g_no_arg;
GLOBAL monash::DynamicWinds* g_dynamic_winds;
GLOBAL monash::InputPort* g_defaultInputPort;
//GLOBAL monash::DefaultProcedures procedures;
GLOBAL monash::OutputPort* g_currentOutputPort;
GLOBAL monash::OutputPort* g_defaultOutputPort;
GLOBAL monash::InputPort* g_currentInputPort;
GLOBAL monash::Environment* g_top_env;
GLOBAL monash::Translator* g_translator;
GLOBAL bool g_gc_initialized GLOBAL_VAL(false);
GLOBAL bool g_batch_mode GLOBAL_VAL(false);


GLOBAL FILE* g_transcript GLOBAL_VAL(NULL);
GLOBAL ::util::HashMap<int>* g_provide_map;

#define SCM_TRUE   g_true
#define SCM_FALSE  g_false
#define SCM_UNDEF  g_undef
#define SCM_NO_ARG g_no_arg
#define SCM_NIL    g_nil
#define SCM_EOF    g_eof


#ifdef MONA
GLOBAL ::monash::MonaTerminal* g_terminal;
#endif

#ifdef MONA
#define LOAD_SCHEME_INTERACTIVE_LIBRARY "(load \"/LIBS/SCHEME/interact.scm\")"
#define LOAD_SCHEME_BATCH_LIBRARY       "(load \"/LIBS/SCHEME/batch.scm\")"
#else
#define LOAD_SCHEME_INTERACTIVE_LIBRARY "(load \"lib/interact.scm\")"
#define LOAD_SCHEME_BATCH_LIBRARY       "(load \"lib/batch.scm\")"
#endif


// notice!
// don't use like below
// SCHEME_WRITE(stream, o->eval);
// o->eval will be called twice!
#ifdef MONA
#include "MonaTerminal.h"
#define SCHEME_WRITE(stream, ...)                                       \
{                                                                       \
    char buf[1024];                                                     \
    sprintf(buf, __VA_ARGS__);                                          \
    if (g_batch_mode)                                                   \
    {                                                                   \
        printf(buf);                                                    \
    }                                                                   \
    else                                                                \
    {                                                                   \
        g_terminal->formatWrite(buf);                                   \
    }                                                                   \
    int __file = fileno(stream);                                        \
    int __out  = fileno(stdout);                                        \
    int __err  = fileno(stderr);                                        \
    if (g_transcript != NULL && (__file == __out || __file == __err))   \
    {                                                                   \
        fprintf(g_transcript, __VA_ARGS__);                             \
        fflush(g_transcript);                                           \
    }                                                                   \
}

#else

#define SCHEME_WRITE(stream, ...)                                       \
{                                                                       \
    fprintf(stream, __VA_ARGS__);                                       \
    int __file = fileno(stream);                                        \
    int __out  = fileno(stdout);                                        \
    int __err  = fileno(stderr);                                        \
    if (g_transcript != NULL && (__file == __out || __file == __err))   \
    {                                                                   \
        fprintf(g_transcript, __VA_ARGS__);                             \
        fflush(g_transcript);                                           \
    }                                                                   \
}

#endif

#define TRANSCRIPT_WRITE(...)                                   \
{                                                               \
    if (g_transcript != NULL)                                   \
    {                                                           \
        fprintf(g_transcript, __VA_ARGS__);                     \
    }                                                           \
}

#define SCM_APPLY1(name, e, ret, arg1)             \
{                                                  \
    Object* proc = Kernel::evalTailOpt((new Variable(name)), e); \
    Cons* args = new Cons;\
    args->append(arg1);                               \
    ret = Kernel::apply(proc, args, e);            \
}

#define SCM_EVAL(proc, e, ret, arg1)               \
{                                                  \
    Cons* args = new Cons;                         \
    args->append(arg1);                            \
    args->append(e);                               \
    ret = Kernel::apply(proc, args, e);            \
}

#define SCM_APPLY_PROC(procName, a, e, ret)                \
{                                                          \
    CAST(Kernel::evalTailOpt((new Variable(procName)), e), Procedure, p); \
    ret = p->apply(arguments, env);                        \
}


// (a b . c)
#define SCM_LIST_CONS(objects, o, ret, lineno)              \
{                                                           \
    ret = new Cons(SCM_NIL, SCM_NIL, lineno);               \
    Cons* p = ret;                                          \
    for (int i = 0; i < objects->size(); i++)               \
    {                                                       \
        p->setCar(objects->get(i));                         \
        if (i == objects->size() - 1)                       \
        {                                                   \
            p->setCdr(o);                                   \
        }                                                   \
        else                                                \
        {                                                   \
            Cons* tmp = new Cons(SCM_NIL, SCM_NIL, lineno); \
            p->setCdr(tmp);                                 \
            p = tmp;                                        \
        }                                                   \
    }                                                       \
}

// (a b c)
#define SCM_LIST(objects, ret, lineno)                                           \
{                                                                                \
    ret = new ::monash::Cons(SCM_NIL, SCM_NIL, lineno);                          \
    ::monash::Cons* p = ret;                                                     \
    for (int i = 0; i < objects->size(); i++)                                    \
    {                                                                            \
        p->setCar(objects->get(i));                                              \
        if (i != objects->size() - 1)                                            \
        {                                                                        \
            ::monash::Cons* tmp = new ::monash::Cons(SCM_NIL, SCM_NIL, lineno);  \
            p->setCdr(tmp);                                                      \
            p = tmp;                                                             \
        }                                                                        \
    }                                                                            \
}

#define FOREACH_LIST(list, kar, kdr, syntax) \
        for (Object *l = list, *kdr = ((Cons*)l)->getCdr(), *kar = ((Cons*)l)->getCar(); \
                 ; kdr->isCons() ? (l = kdr, kdr = ((Cons*)l)->getCdr(), kar = ((Cons*)l)->getCar()): (RAISE_ERROR(list->lineno(), "syntax-error: malformed %s", syntax), kar = NULL))

#endif // __SCHEME_H__

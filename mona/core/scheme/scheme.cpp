/*!
    \file   scheme.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#define GLOBAL_VALUE_DEFINED
#include "scheme.h"

using namespace util;
using namespace monash;

#include "MonaTerminal.h"
#include "primitive_procedures.h"

void scheme_init()
{
#ifdef USE_MONA_GC
    gc_init();
#endif

#ifndef MONA
    scheme_expand_stack(64);
#endif
    cont_initialize();
}

void scheme_const_init()
{
    g_translator        = new Translator();
    g_defaultOutputPort = new OutputPort(stdout);
    g_defaultInputPort  = new InputPort("stdin", stdin);
    g_currentInputPort  = g_defaultInputPort;
    g_currentOutputPort = g_defaultOutputPort;
    g_transcript        = NULL;
    g_dynamic_winds     = new DynamicWinds;
    g_true              = True::createInstance();
    g_false             = False::createInstance();
    g_undef             = Undef::createInstance();
    g_no_arg            = NULL;
    g_nil               = Nil::createInstance();
    g_eof               = Eof::createInstance();
}

void scheme_register_primitives(Environment* env)
{
    scheme_const_init();
#include "register.inc"
    env->defineVariable(new Variable("#f"),   g_false);
    env->defineVariable(new Variable("#t"),   g_true);
}

void scheme_expand_stack(uint32_t mb)
{
#ifndef MONA
    struct rlimit r;
    getrlimit(RLIMIT_STACK, &r);
    r.rlim_cur = mb * 1024 * 1024;
    setrlimit(RLIMIT_STACK, &r);
    getrlimit(RLIMIT_STACK, &r);
#endif
}

Object* scheme_eval_string(const String& input, Environment* env, bool out /* = false */)
{
    StringReader* reader = new StringReader(input);
    Scanner* scanner = new Scanner(reader, reader, NULL);
    Parser parser(scanner);
    Object* evaluated = NULL;
    for (Object* sexp = parser.parse(); sexp != SCM_EOF; sexp = parser.parse())
    {
        evaluated = Kernel::eval(sexp, env);
        if (out) SCHEME_WRITE(stdout, "%s\n", evaluated->toString().data());
    }
    return evaluated;
}

Environment* env;

int scheme_batch(const String& file)
{
    g_batch_mode = true;
    String input = load(file);
    if (input == "")
    {
        fprintf(stderr, "can not load: %s file\n", file.data());
        return -1;
    }
    Error::exitOnError();
    Error::file = file;
    Environment* env = new Environment();
    SCM_ASSERT(env);
    g_top_env = env;
    scheme_register_primitives(env);
// todo あとで戻す
    scheme_eval_string(LOAD_SCHEME_BATCH_LIBRARY, env, false);
    scheme_eval_string(input, env);
    return 0;
}

void scheme_interactive()
{
    env = new Environment();
    Interaction* interaction = new Interaction(env);
#ifdef MONA
    g_terminal = new monash::MonaTerminal();
#endif
    SCM_ASSERT(env);
    g_top_env = env;
    scheme_register_primitives(env);
    RETURN_ON_ERROR("stdin");
// todo あとで戻す
    scheme_eval_string(LOAD_SCHEME_INTERACTIVE_LIBRARY, env, false);
    interaction->showPrompt();

#ifdef MONA
    for (;;)
    {
        // insert '(' automatically.y
        g_terminal->outputChar('(');
        interaction->onInput(g_terminal->getLine());
    }
#else

    for (;;)
    {
        char* line = NULL;;
        size_t length = 0;
        getline(&line, &length, stdin);
        interaction->onInput(line);
    }
#endif
}

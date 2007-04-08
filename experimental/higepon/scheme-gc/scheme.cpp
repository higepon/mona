#define GLOBAL_VALUE_DEFINED
#include "scheme.h"

using namespace util;
using namespace monash;

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
    g_defaultOutputPort = new OutputPort(stdout);
    g_defaultInputPort  = new InputPort("stdin", stdin);
    g_currentInputPort  = g_defaultInputPort;
    g_currentOutputPort = g_defaultOutputPort;
    g_transcript = NULL;
    g_dynamic_winds = new DynamicWinds;
    g_true = new True;
    g_false = new False;
    g_undef = new Undef;
    g_no_arg = new Objects;
    g_nil = new Nil;
    g_eof = new Eof;
}

void scheme_register_primitives(Environment* env)
{
    scheme_const_init();
#include "register.inc"
    env->defineVariable(new Variable("#f"),   g_false);
    env->defineVariable(new Variable("#t"),   g_true);
    env->defineVariable(new Variable("set!"), new Set());
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

uint32_t count_char(const char* s, char c)
{
    int length = strlen(s);
    uint32_t count = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        if (s[i] == c) count++;
    }
    return count;
}

Object* scheme_eval_string(String& input, Environment* env, bool out /* = false */)
{
    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);
    StringReader* reader = new StringReader(input);
    Scanner* scanner = new Scanner(reader);
    ExtRepParser parser(scanner);
    Object* evalFunc = (new Variable("eval"))->eval(env);
    Object* evaluated = NULL;
    for (Object* sexp = parser.parse(); sexp != SCM_EOF; sexp = parser.parse())
    {
        SCM_EVAL(evalFunc, env, evaluated, sexp);
//        _printf("sexp %s\n", evaluated->toString().data());
        if (out) SCHEME_WRITE(stdout, "%s\n", evaluated->toString().data());
    }
    return evaluated;
}
int scheme_exec_file(const String& file)
{
    String input = load(file);
    if (input == "")
    {
        fprintf(stderr, "can not load: %s file\n", file.data());
        return -1;
    }
    Error::exitOnError();
    Error::file = file;
    MacroFilter f;
    Translator translator;
    Environment* env = new Environment(f, translator);
    SCM_ASSERT(env);
    g_top_env = env;
    scheme_register_primitives(env);
    scheme_eval_string(input, env);
    return 0;
}

void scheme_input_loop()
{
    MacroFilter f;
    Translator translator;
    Environment* env = new Environment(f, translator);
    SCM_ASSERT(env);
    g_top_env = env;
    scheme_register_primitives(env);

#ifdef MONA
    char line[1024];
#else
    char* line = NULL;;
    size_t length = 0;
#endif

    uint32_t open_paren_count = 0;
    uint32_t close_paren_count = 0;
    bool show_prompt = true;

    RETURN_ON_ERROR("stdin");

#ifdef MONA
    String input = "(load \"/servers/MONA.SCM\")";
#else
    String input = "(load \"lib/MONA.SCM\")";
#endif
    scheme_eval_string(input, env);
    input = "";

    for (;;)
    {
        if (show_prompt) SCHEME_WRITE(stdout, "mona> ");
#ifdef MONA
        monapi_stdin_read((uint8_t*)line, 1024);
#else
        getline(&line, &length, stdin);
#endif
        open_paren_count += count_char(line, '(');
        close_paren_count += count_char(line, ')');
        input += line;
        if (input != "" && open_paren_count == close_paren_count)
        {
            TRANSCRIPT_WRITE(input.data());
            scheme_eval_string(input, env, true);
            open_paren_count = 0;
            close_paren_count = 0;
            show_prompt = true;
            input = "";
        }
        else
        {
            show_prompt = false;
        }
    }
}

SExp* objectToSExp(Object* o)
{
    SExp* sexp = NULL;
    if (o->isNumber())
    {
        sexp = new SExp(SExp::NUMBER, o->lineno());
        Number* n = (Number*)o;
        sexp->value = n->value();
    }
    else if (o->isNil())
    {
        sexp = new SExp(SExp::SEXPS, o->lineno());
    }
    else if (o->isCharcter())
    {
        sexp = new SExp(SExp::CHAR, o->lineno());
        Charcter* c = (Charcter*)o;
        sexp->text = c->stringValue();

    }
    else if (o->isVector())
    {
        sexp = new SExp(SExp::SEXPS, o->lineno());
        ::monash::Vector* v = (::monash::Vector*)o;
        SExp* vectorStart = new SExp(SExp::SYMBOL, o->lineno());
        vectorStart->text = "vector";
        sexp->sexps.add(vectorStart);
        for (uint32_t i = 0; i < v->size(); i++)
        {
            sexp->sexps.add(objectToSExp(v->get(i)));
        }
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
        sexp->text = r->text();
    }
    else if (o->isPair())
    {
        ::monash::Pair* p = (::monash::Pair*)o;
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

SExp* pairToSExp(monash::Pair* p)
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
            ::monash::Pair* p = (::monash::Pair*)o;
            sexp->sexps.add(::objectToSExp(p->getCar()));
            if (!p->getCdr()->isPair() && !p->getCdr()->isNil())
            {
                SExp* s = new SExp(SExp::SYMBOL);
                s->text = ".";
                sexp->sexps.add(s);
                sexp->sexps.add(::objectToSExp(p->getCdr()));
                break;
            }
            o = p->getCdr();
        }
        else
        {
            sexp->sexps.add(::objectToSExp(o));
            break;
        }
    }
    return sexp;
}

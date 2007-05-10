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
    for (int i = 0; i < length; i++)
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

String scheme_prompt(Environment* env)
{
    String input = "(mona-prompt-string)";
    Object* o = scheme_eval_string(input, env, false);
    if (o->isSString())
    {
        SString* s = (SString*)o;
        return s->value();
    }

    else
    {
        RAISE_ERROR(o->lineno(), "(mona-prompt-string) not defined");
        return "";
    }
}

static MacroFilter f;
static Translator translator;
static Environment* env;
static String input = "";
bool scheme_on_input_line(const String& line)
{
    input += line;
    if (count_char(input.data(), '(') != count_char(input.data(), ')'))
    {
        return false;
    }
    TRANSCRIPT_WRITE(input.data());
    scheme_eval_string(input, env, false);
    mona_shell_add_history(input);
    input = "";

    SCHEME_WRITE(stdout, scheme_prompt(env).data());
    return false;
}

void scheme_on_reedit()
{
    input = "";
    mona_shell_reedit();
    SCHEME_WRITE(stdout, scheme_prompt(env).data());
}
#ifdef MONA
using namespace MonAPI;
extern bool suppressKey;
#endif
void scheme_interactive()
{
    env = new Environment(f, translator);
    SCM_ASSERT(env);
    g_top_env = env;
    scheme_register_primitives(env);

    RETURN_ON_ERROR("stdin");
#ifdef MONA
    input = "(load \"/LIBS/SCHEME/scheme.scm\")";
#else
    input = "(load \"lib/scheme.scm\")";
#endif
    scheme_eval_string(input, env, false);
    SCHEME_WRITE(stdout, scheme_prompt(env).data());

#ifdef MONA
    // errorで戻ってくる場合があるので変数を初期化
    input = "";
    mona_shell_init_variables();
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;
        switch (msg.header)
        {
            case MSG_KEY_VIRTUAL_CODE:
                if (!suppressKey && (msg.arg2 & KEY_MODIFIER_DOWN) != 0)
                {
                    mona_shell_on_key_down(msg.arg1, msg.arg2);
                }
                else if (msg.arg1 == 0)
                {
                    mona_shell_on_key_down(msg.arg2, msg.arg3);
                }
                break;
            case MSG_CHANGE_OUT_STREAM_BY_HANDLE:
            {
                MessageInfo m;
                uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
                if (targetID == THREAD_UNKNOWN || Message::sendReceive(&m, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
                    printf("SCREEN.EX5 not found\n");
                    continue;
                }
                uint32_t screenHandle = m.arg2;
                terminal_ = new terminal::Util(Stream::FromHandle(msg.arg1));
                Message::reply(&msg, screenHandle);
                break;
            }

        default:
                break;
        }
    }
#else
    for (;;)
    {
        char* line = NULL;;
        size_t length = 0;
        getline(&line, &length, stdin);
        scheme_on_input_line(line);
    }
#endif
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

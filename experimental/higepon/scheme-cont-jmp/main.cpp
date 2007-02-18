/*!
  \file   main.cpp
  \brief  scheme

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/06/16 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "scheme.h"
#include <stdio.h>
#include <time.h>

using namespace util;
using namespace monash;


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

void input_loop()
{
    QuoteFilter quoteFilter;
    MacroFilter f;
    Translator translator;
    Environment* env = new Environment(f, translator);ASSERT(env);
    registerPrimitives(env);

    char* line = NULL;;
    size_t length = 0;
    uint32_t open_paren_count = 0;
    uint32_t close_paren_count = 0;
    bool show_prompt = true;
    RETURN_ON_ERROR();
//        Error::returnOnError();
    String input = "(load \"test/scheme.scm\")";
    input = quoteFilter.filter(input);
    input = "(" + input + " )";
    SExp* allSExp = SExp::fromString(input);
    SExps sexps = allSExp->sexps;
    for (int i = 0; i < sexps.size(); i++)
    {
        SExp* sexp = sexps.get(i);
        f.filter(sexp);
        Object* object = NULL;
        translator.translate(&sexp, &object);
        object->eval(env);
    }
    input = "";

    for (;;)
    {
        if (show_prompt) SCHEME_WRITE(stdout, "mona> ");
        getline(&line, &length, stdin);
        open_paren_count += count_char(line, '(');
        close_paren_count += count_char(line, ')');
        input += line;
        if (input != "" && open_paren_count == close_paren_count)
        {
            input = quoteFilter.filter(input);
            TRANSCRIPT_WRITE(input.data());
            input = String("(") + input + " )";
            SExp* allSExp = SExp::fromString(input);
            SExps sexps = allSExp->sexps;
            for (int i = 0; i < sexps.size(); i++)
            {
                SExp* sexp = sexps.get(i);
                f.filter(sexp);

                Object* object = NULL;

//                sexp->print();
                if (translator.translate(&sexp, &object) != Translator::SUCCESS)
                {
//                    fprintf(stderr, "translate error \n");
                    open_paren_count = 0;
                    close_paren_count = 0;
                    input = "";
                    break;
                }
                // let's eval!
                Object* evaluated = object->eval(env);
                SCHEME_WRITE(stdout, "%s\n", evaluated->toStringValue().data());

            }
                    open_paren_count = 0;
                    close_paren_count = 0;
                    show_prompt = true;
                    input = "";

        }
        else
        {
            show_prompt = false;
            //          printf("diff\n");
//        printf("%s[%d]\n", line, length);
        }

    }

}

int main(int argc, char *argv[])
{
    
    cont_initialize();
    if (argc == 1)
    {
        input_loop();
        return 0;
    }

    String input = load(argv[1]);
    if (input == "")
    {
        fprintf(stderr, "can not load: %s file\n", argv[1]);
        return -1;
    }

    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);
//    printf("%s", input.data());
    Error::exitOnError();
    Error::file = argv[1];

    MacroFilter f;
    Translator translator;
    Environment* env = new Environment(f, translator);ASSERT(env);
    registerPrimitives(env);

    input = "(" + input + " )";
    SExp* allSExp = SExp::fromString(input);

    SExps sexps = allSExp->sexps;

//     printf("%s\n", allSExp->toZSExp()->toString().data());

// load
//    sexp->execLoadSyntaxes();
    for (int i = 0; i < sexps.size(); i++)
    {
        SExp* sexp = sexps.get(i);
        f.filter(sexp);

        Object* object = NULL;

        if (translator.translate(&sexp, &object) != Translator::SUCCESS)
        {
            SCHEME_WRITE(stderr, "translate error \n");
            return -1;
        }
        // let's eval!
        object->eval(env);

//         Objects* args = new Objects;
//         for (Continuation* c = popContinuation(); c != NULL; c = popContinuation())
//         {
//             Object* o = c->object;
//             o = o->eval(env);
//             if (o->isCompoundProcedure() || o->isPrimitiveProcedure())
//             {
//                 Object*ret = Scheme::apply(o, args, env, NULL);
//                 printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//                 printf("[ret]%s\n", ret->toString().data());
//             }
//             else
//             {
//                 args->add(o);
//             }
//         }

    }

    return 0;
}

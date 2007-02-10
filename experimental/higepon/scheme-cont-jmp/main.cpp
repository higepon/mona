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
#include "libcont/cont.h"


using namespace monash;
using namespace std;

uint32_t count_char(const char* s, char c)
{
    uint32_t length = strlen(s);
    uint32_t count = 0;
    for (uint32_t i = 0; i < length; i++)
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
    string input = "(load \"test/scheme.scm\")";
    input = quoteFilter.filter(input);
    input = "(" + input + " )";
    SExp* allSExp = SExp::fromString(input);
    SExps sexps = allSExp->sexps;
    for (SExps::iterator p = sexps.begin(); p != sexps.end(); ++p)
    {
        SExp* sexp = (*p);
        f.filter(sexp);
        Object* object = NULL;
        translator.translate(&sexp, &object);
        object->eval(env);
    }
    input = "";

    for (;;)
    {

        if (show_prompt) printf("mona> ");
        getline(&line, &length, stdin);
        open_paren_count += count_char(line, '(');
        close_paren_count += count_char(line, ')');
        input += line;
        if (input != "" && open_paren_count == close_paren_count)
        {
            input = quoteFilter.filter(input);
//            printf("input[%s]\n", input.c_str());
            input = "(" + input + " )";
            SExp* allSExp = SExp::fromString(input);
            SExps sexps = allSExp->sexps;
            for (SExps::iterator p = sexps.begin(); p != sexps.end(); ++p)
            {
                SExp* sexp = (*p);
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
                printf("%s\n", object->eval(env)->toStringValue().c_str());
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

    string input = load(argv[1]);
    if (input == "")
    {
        fprintf(stderr, "can not load: %s file\n", argv[1]);
        return -1;
    }

    QuoteFilter quoteFilter;
    input = quoteFilter.filter(input);
//    printf("%s", input.c_str());
    Error::exitOnError();
    Error::file = argv[1];

    MacroFilter f;
    Translator translator;
    Environment* env = new Environment(f, translator);ASSERT(env);
    registerPrimitives(env);

    input = "(" + input + " )";
    SExp* allSExp = SExp::fromString(input);

    SExps sexps = allSExp->sexps;

//     printf("%s\n", allSExp->toZSExp()->toString().c_str());

// load
//    sexp->execLoadSyntaxes();
    for (SExps::iterator p = sexps.begin(); p != sexps.end(); ++p)
    {
        SExp* sexp = (*p);
        f.filter(sexp);

        Object* object = NULL;

        if (translator.translate(&sexp, &object) != Translator::SUCCESS)
        {
            fprintf(stderr, "translate error \n");
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
//                 printf("[ret]%s\n", ret->toString().c_str());
//             }
//             else
//             {
//                 args->push_back(o);
//             }
//         }

    }

    return 0;
}

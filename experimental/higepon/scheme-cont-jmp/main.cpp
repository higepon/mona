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


using namespace monash;
using namespace std;


ZSExp* eval(ZSExp* exp)
{
    // self evaluate

    
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return -1;
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
    Error::initialize();
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

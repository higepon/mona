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

    Error::initialize();
    Error::file = argv[1];

    MacroFilter f;
    Translator translator;
    Environment* env = new Environment(f, translator);ASSERT(env);
    registerPrimitives(env);

    input = "(" + input + " )";
    SExp* allSExp = SExp::fromString(input);
    SExps sexps = allSExp->sexps;

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
    }

    return 0;
}

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

    // todo ugly fixme
    input = "(begin " + input + " )";

    Node* node = Node::fromString(input);
    node->execLoadSyntaxes();

    MacroFilter f;
    f.filter(node);

    Object* object = NULL;
    Translator translator;
    if (translator.translate(&node, &object) != Translator::SUCCESS)
    {
        fprintf(stderr, "translate error \n");
        return -1;
    }
    Environment* env = new Environment(f);ASSERT(env);
    registerPrimitives(env);

    // let's eval!
    object->eval(env);
    return 0;
}

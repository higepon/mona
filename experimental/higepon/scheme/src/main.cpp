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
        return -1;
    }

    Tokenizer tokenizer(input);
    Parser parser(&tokenizer);
    Node* node = parser.parse();

//    node->print();

    Object* object = NULL;
    if (Translator::translate(node, &object) != Translator::SUCCESS)
    {
        fprintf(stderr, "translate error \n");
        return -1;
    }
    Environment* environment = new Environment();
    environment->defineVariable(new Variable("+"), new Plus());
    environment->defineVariable(new Variable("-"), new Minus());
    environment->defineVariable(new Variable("="), new NumberEqual());
    environment->defineVariable(new Variable(">"), new NumberGt());
    environment->defineVariable(new Variable("cons"), new Cons());
    environment->defineVariable(new Variable("car"), new Car());
    environment->defineVariable(new Variable("cdr"), new Cdr());
    environment->defineVariable(new Variable("display"), new Display());
    environment->defineVariable(new Variable("string?"), new StringP());
    environment->defineVariable(new Variable("number?"), new NumberP());
    environment->defineVariable(new Variable("string->number"), new StringToNumber());
    environment->defineVariable(new Variable("#f"), new Number(0));
    environment->defineVariable(new Variable("#t"), new Number(1));
    object->eval(environment);
    return 0;
}

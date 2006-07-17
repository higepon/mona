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

    node->print();

    Object* object = NULL;
    if (Translator::translate(node, &object) != Translator::SUCCESS)
    {
        return -1;
    }
    Environment* environment = new Environment();
    display(object->eval(environment));
    return 0;
}

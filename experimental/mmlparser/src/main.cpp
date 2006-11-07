/*!
  \file   main.cpp
  \brief  mmlparser

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/11/04 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "types.h"
#include "Parser.h"
#include "Config.h"
#include <stdio.h>

//  memo for memory leak check
//  valgrind --leak-check=full -v --show-reachable=yes ./src/mmlparser 2> val.log

using namespace MML;

int main(int argc, char *argv[])
{
    const char* mml = "T155Q7L8O4\n"
        "RGGGE-2.RFFFD4&D1\n"
        "RGGGQ8E-Q7A-A-A-Q8GQ7>E-E-E-C8&C2<GGG\n"
        "Q8DQ7A-A-A-Q8GQ7>FFFD8&D2GGF\n"
        "Q8E-Q7<E-E-FQ8G>Q7GGFQ8E-Q7<E-E-F\n"
        "Q8GQ7>GGFL4E-RCRG2.L8R\n";

    Parser parser;
    Elements* elements = parser.parse(mml);

    for (Elements::iterator it = elements->begin(); it != elements->end(); it++)
    {
        Element* e = *it;
        if (e->isRest())
        {
            printf("[Rest]%dms", e->ms);
        }
        else
        {
            printf("[%dhz]%dms", e->hz, e->ms);
        }
        delete (e);
    }

    delete elements;
    return 0;
}

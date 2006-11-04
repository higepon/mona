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

using namespace MML;

typedef struct
{
    dword tempo;

} Config;



int main(int argc, char *argv[])
{
    char* mml = "T155Q7L8O4\n"
        "RGGGE-2.RFFFD4&D1\n"
        "RGGGQ8E-Q7A-A-A-Q8GQ7>E-E-E-C8&C2<GGG\n"
        "Q8DQ7A-A-A-Q8GQ7>FFFD8&D2GGF\n"
        "Q8E-Q7<E-E-FQ8G>Q7GGFQ8E-Q7<E-E-F\n"
        "Q8GQ7>GGFL4E-RCRG2.L8R\n";

    Config config;

    Parser parser(&config, mml);
    parser.parse();


    return 0;
}

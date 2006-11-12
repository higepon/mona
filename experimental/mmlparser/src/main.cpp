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

const char* mml2 = "t70"
"eeab>c<ba2 ffede2.r"
"eeab>c<ba2 fde.e8<a2.>r"
"cc<ba>f.f8e2 def.f8e2.r"
    "eeab>c<ba2 fde.e8<a2.>r\n";

const char* mml3 = "T210O4L4"
"F+F+GA AGF+E DDEF+ F+.E8E2"
"F+F+GA AGF+E DDEF+ E.D8D2"
"EEF+D EF+8G8F+D EF+8G8F+E DE<A>F+&"
    "F+F+GA AGF+E DDEF+ E.D8D2"
""
"O5"
"F+F+GA AGF+E DDEF+ F+.E8E2"
"F+F+GA AGF+E DDEF+ E.D8D2"
"EEF+D EF+8G8F+D EF+8G8F+E DE<A>F+&"
    "F+F+GA AGF+E DDEF+ E.D8D2\n";

const char* mml_multi = ""
    "[Channel1]"
    "T180V15L8<<FCDF4CFE4C-EA4EAED<A>CD4<A>DC4<G>CF4CC<BA+A+A+A+A+FGA+AAA>D4<A>D<AG+G+"
    ">CD+GFD+C4C4C4CDF4FFFFCDE4C-D<A4EA>C+D<AB>D4<ABB+4G>CF4CDF<A+A+A+A+A+A+A+A+AAA>D4"
    "<AA>D<GGAAA+A+BB>CC<AAA+A+>CCC+4C+C+4C+4C+C+4C+C+4C+4C+<E4EE4EE4>E2.&ED+4ED+4.ED+4.ED+ED+N22A+4"
    "[Channel2]"
    "T180V12L8<FF4FF4FE4DEE2&EFF4FF4FG4FGA4.AAG4FD4.AAG4FC4.AAG4FD+4D4E1&EFF4FF4FE4DE"
    "E2&EFF4FF4FG4FGA4.AAG4FL4D.AAG8FD.GGGL8GAF4FE4.C+D+F4FD+4C+1&C+C+D+E4ED+4<B2&BG+4G+B4A+4BA+4.BA+4.BA+BA+2"
    "[Channel3]"
    "T180V12L8<AA4AA4AG4FGA2&AAA4AA4AA+4AA+>C4.CC<A+4AF4.>CC<A+4AF4.>CC<A+4G+G4F4G1&GAA4AA4AG4FGA2&AAA4AA4AA+4A"
    "A+>C4.CCL4<A+A8F.>CD<A8GF.A+A+A+L8A+B+A4AG4.FGG+4G+G4F1&FFGG+4G+F+4E2&E<B4B>E4D+4ED+4.ED+4.ED+ED+2\n";

    Parser parser;
    Channels* channels = parser.parse(mml_multi);

    for (Channels::iterator it = channels->begin(); it != channels->end(); it++)
    {
        Channel* channel = *it;
        printf("[channel]\n");
        for (Channel::iterator eit = channel->begin(); eit != channel->end(); eit++)
        {
            Element* e = *eit;
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
        delete channel;
        printf("\n");
    }
    delete channels;

    return 0;
}

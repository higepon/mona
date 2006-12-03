/*!
  \file   main.cpp
  \brief  dynamic_code5

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/12/02 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

/*!
  \file   main.cpp
  \brief  dynamic_code5

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/12/02 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>

void hello()
{
    printf("%s\n", __func__);
}

void goodbye()
{
    printf("%s\n", __func__);
}

int main(int argc, char *argv[])
{
    unsigned char code[128];
    code[0] = 0x55;
    code[1] = 0x89;
    code[2] = 0xE5;
    code[3] = 0xB8;
    *(unsigned int*)(&code[4]) = (unsigned int)hello;
    code[8] = 0xFF;
    code[9] = 0xD0;
    code[10] = 0xB8;
    *(unsigned int*)(&code[11]) = (unsigned int)goodbye;
    code[16] = 0xFF;
    code[17] = 0xD0;
    code[15] = 0xC9;
    code[18] = 0xC3;
    void (*call_func)(void) = (void (*)(void))code;
    (*call_func)();
    return 0;
}


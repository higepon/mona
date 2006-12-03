/*!
  \file   main.cpp
  \brief  dynamic_code4

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
#include <stdlib.h>
#include <string.h>

void hello()
{
    printf("hello\n");
}


int main(int argc, char *argv[])
{
    unsigned char code[] = {0x55, 0x89, 0xE5, 0xB8, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD0, 0xC9, 0xC3};
    *(unsigned int*)(&code[4]) = (unsigned int)hello;
    void (*call_func)(void) = (void (*)(void))code;
    (*call_func)();
    return 0;
}

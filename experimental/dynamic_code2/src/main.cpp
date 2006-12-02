/*!
  \file   main.cpp
  \brief  dynamic_code2

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/12/01 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>

extern "C" void call_func(unsigned int func);

void hello()
{
    printf("hello\n");
}

int main(int argc, char *argv[])
{
    unsigned int f = (unsigned int )hello;
    call_func(f);
    return 0;
}

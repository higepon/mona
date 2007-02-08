/*!
  \file   main.c
  \brief  libcont

  Copyright (c) 2007 higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2007/01/30 update:$Date$
*/

#include <stdio.h>
#include <stdint.h>

void jmp_outside_test();
void jmp_inside_test();

void cont_initialize();

int main(int argc, char *argv[])
{
    cont_initialize();
    jmp_outside_test();
    jmp_inside_test(); // may be segmentation fault with setjmp!
    printf("test done\n");
    return 0;
}

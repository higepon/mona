/*!
  \file   main.cpp
  \brief  libcont

  Copyright (c) 2007 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2007/01/30 update:$Date$
*/

#include <stdio.h>
#include <stdint.h>

void jmp_outside_test();
void jmp_inside_test();


//uint32_t cont_stack_bottom;

void     cont_initialize();

int main(int argc, char *argv[])
{
// stack の底をどう取得するかなぁ
//    cont_stack_bottom = (uint32_t)get_stack_pointer() + 50;
    cont_initialize();
    jmp_outside_test();
    jmp_inside_test(); // may be segmentation fault!
    printf("***********\n");fflush(stdout);
    return 0;
}

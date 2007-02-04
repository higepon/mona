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

void* get_stack_pointer()
{
    register void* stack_pointer asm ("%esp");
    return (void*)((unsigned int)stack_pointer + 8);
}

uint32_t stack_bottom;

int main(int argc, char *argv[])
{
    stack_bottom = (uint32_t)get_stack_pointer() + 50;
    jmp_outside_test();
    jmp_inside_test(); // may be segmentation fault!
    printf("***********\n");fflush(stdout);
    return 0;
}

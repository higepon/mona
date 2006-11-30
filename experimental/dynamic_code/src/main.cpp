/*!
  \file   main.cpp
  \brief  dynamic_code

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/11/30 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>

#define ENTRY_POINT_FUNC(x)      \
    void entry_point_##x()       \
    {                            \
        printf("%s\n", __func__);\
    }

ENTRY_POINT_FUNC(0);
ENTRY_POINT_FUNC(1);
ENTRY_POINT_FUNC(2);

typedef unsigned int dword;

void call_functions(dword size, dword* functions, dword goal)
{
    for (dword i = 0; i < size; i++)
    {
        void (*f)(void) = (void(*)(void))functions[i];
        f();
    }
    void (*g)(void) = (void(*)(void))goal;
    g();
}

void hello()
{
    printf("hello\n");
}

int main(int argc, char *argv[])
{
    dword functions[3];
    functions[0] = (dword)entry_point_0;
    functions[1] = (dword)entry_point_1;
    functions[2] = (dword)entry_point_2;

    call_functions(3, functions, (dword)hello);
    return 0;
}

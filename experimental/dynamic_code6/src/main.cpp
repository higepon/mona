/*!
  \file   main.cpp
  \brief  dynamic_code6

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

void hello()
{
    printf("%s\n", __func__);
}

void goodbye()
{
    printf("%s\n", __func__);
}

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;


byte* make_code(void (**functions)(void), dword num, dword* size)
{
    *size = 3 + 2 + 7 * num;
    byte* code = new byte[*size];
    code[0] = 0x55;
    code[1] = 0x89;
    code[2] = 0xE5;
    byte* start = &code[3];
    for (dword i = 0; i < num; i++)
    {
        start[i * 7] = 0xB8;
        *(dword*)(&start[i * 7 + 1]) = (dword)functions[i];
        start[i * 7 + 5] = 0xFF;
        start[i * 7 + 6] = 0xD0;
    }
    start[7 * num] = 0xC9;
    start[7 * num + 1] = 0xC3;
    return code;
}


int main(int argc, char *argv[])
{
    void (*functions[2])(void);
    functions[0] = hello;
    functions[1] = goodbye;

    dword size;
    byte* code = make_code(functions, 2, &size);
    void (*f)(void) = (void (*)(void))code;
    (*f)();

    delete[] code;
    return 0;
}

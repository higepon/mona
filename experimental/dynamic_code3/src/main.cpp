/*!
  \file   main.cpp
  \brief  dynamic_code3

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

unsigned char code[] = {0x55,0x89,0xE5,0x81,0xEC,0x08,0x00,0x00,0x00,0x8B,0x45,0x08,0xFF,0xD0,0xC9,0xC3};

void hello()
{
    printf("hello\n");
}

int main(int argc, char *argv[])
{
    unsigned char* p = (unsigned char*)malloc(sizeof(code));
    memcpy(p, code, sizeof(code));
    void (*call_func)( void (*)(void) ) = (void (*)( void (*)(void) ))code;
    (*call_func)(hello);
    free(p);
    return 0;
}

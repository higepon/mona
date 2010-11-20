/*!
  \file   main.cpp
  \brief  message header convert for Mona.

  Copyright (c) 2002- 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/2/16 update:$Date$
*/
#include <stdio.h>
#include "MessageUtil.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: message2header \"FIL:OPN\"\n");
        return -1;
    }
    MessageUtil util;
    printf("0x%x /* \"%s\" */\n", util.stringToDword(argv[1]), argv[1]);
    return 0;
}


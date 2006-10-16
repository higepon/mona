/*!
  \file   MessageUtil.cpp
  \brief  message header convert for Mona.

  Copyright (c) 2002- 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/2/16 update:$Date$
*/

#include <ctype.h>
#include <string.h>
#include "MessageUtil.h"

MessageUtil::MessageUtil()
{
}

MessageUtil::~MessageUtil()
{
}

byte MessageUtil::charTo5bit(char c)
{
    c = toupper(c);
    if ('A' <= c && c <= 'Z')
    {
        return (byte)(c - 'A');
    }
    else if ('1' <= c && c <= '5')
    {
        return (byte)(c - '1' + 26);
    }
    else if (' ' == c)
    {
        return 31;
    }
    else
    {
        return 0;
    }
    return (byte)c;
}

dword MessageUtil::stringToDword(const char* str, bool reply /* = false */)
{
    if (!str) return 0;
    dword length = strlen(str);
    if (length < 7) return 0;
    dword result = charTo5bit(str[0]) << 27
        | charTo5bit(str[1]) << 22
        | charTo5bit(str[2]) << 17
        | charTo5bit(str[4]) << 12
        | charTo5bit(str[5]) << 7
        | charTo5bit(str[6]) << 2
        | ((str[3] == ':') ? 2 : 0)
        | (reply ? 1 : 0);
    return result;
}

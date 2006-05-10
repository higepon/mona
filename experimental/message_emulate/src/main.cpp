/*!
  \file   main.cpp
  \brief  message emulate

  Copyright (c) 2002- 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/2/16 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include "MessageList.h"

int main(int argc, char *argv[])
{
    MessageList messageList;
    Message m1, m2, m3, m4, message;
    m1.text = "hello1";
    m2.text = "hello2";
    m3.text = "hello3";
    m4.text = "hello4";
    messageList.push(m1);
    messageList.push(m2);
    messageList.push(m3);
    messageList.push(m4);

    // pop
    if (messageList.exist()) {
        message = messageList.pop();
    }
    printf("%s\n", message.text.c_str());

    // peek
    printf("size=%d\n", messageList.length());
    message = messageList.peek(1);
    printf("%s\n", message.text.c_str());
    printf("size=%d\n", messageList.length());
    messageList.remove(1);
    printf("size=%d\n", messageList.length());
    return 0;
}



/*!
    \file  MessageServer.h
    \brief MessageServer

    class MessageServer

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/29 update:$Date$
*/
#ifndef _MONA_MESSAGE_SERVER_
#define _MONA_MESSAGE_SERVER_

#include <Message.h>

class MessageServer {

  public:
    MessageServer();
    ~MessageServer();
    void Main();
    void send(Message* message);
    bool hasMessage();

  private:
    Message* getMesasge();

  private:
    Message queue_;
};

#endif

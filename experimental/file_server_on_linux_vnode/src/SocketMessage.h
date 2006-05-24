#ifndef _MONA_USERLIB_SOCKET_MESSAGE_
#define _MONA_USERLIB_SOCKET_MESSAGE_

#ifdef ON_LINUX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "Message.h"
#include "types.h"
#else
#include <sys/types.h>
#endif

namespace MonAPI {

/*----------------------------------------------------------------------
    SocketMessage
----------------------------------------------------------------------*/
class SocketMessage {

public:
    static int createReceiveSocket();
    static int setMyHost(struct sockaddr_in* me);
    static int send(dword tid, MessageInfo* info);
    static int receive(MessageInfo* info);
    static void initialize();
    static int port;
};

}

#endif

/*----------------------------------------------------------------------
    SocketMessage
----------------------------------------------------------------------*/
#include "SocketMessage.h"
#include "types.h"
#include "ProcessUtil.h"

namespace MonAPI {

using namespace mona_helper;
int SocketMessage::port;

void SocketMessage::initialize()
{
    pid_t pid = getpid();

    // well-known port
    if (pid < 1024 || pid > 65535)
    {
        fprintf(stderr, "port error\n");
        exit(1);
    }
    SocketMessage::port = pid;
}

int SocketMessage::setMyHost(struct sockaddr_in* me)
{
    char hostname[257];
    struct hostent* myhost;
    gethostname(hostname, 256);
    if (NULL == (myhost = gethostbyname(hostname))) {
        perror("gethostbyname");
        return -1;
    }
    bcopy(myhost->h_addr, &me->sin_addr, myhost->h_length);
    return 0;
}

int SocketMessage::createReceiveSocket()
{
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("receive:socket()");
        exit(-1);
        return -1;
    }

    const int one = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    struct sockaddr_in me;
    bzero((char *)&me, sizeof(me));
    SocketMessage::setMyHost(&me);
    me.sin_family = AF_INET;
    me.sin_port   = htons(SocketMessage::port);
    if (-1 == bind(sock, (struct sockaddr *)&me, sizeof(me)))
    {
        perror("bind");
        exit(-1);
//        return -1;
    }

    if (-1 == (listen(sock, 5)))
    {
        perror("listen");
        return -1;
    }
    return sock;
}

int SocketMessage::send(dword tid, MessageInfo* info)
{
    struct sockaddr_in me;
    bzero((char *)&me, sizeof(me));
    SocketMessage::setMyHost(&me);
    me.sin_family = AF_INET;
    me.sin_port   = htons(tid);
    info->from = SocketMessage::port;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("send:socket()");
        return -1;
    }

    if (-1 == connect(sock, (sockaddr*)&me , sizeof(me)))
    {
        close(sock);
        return -2;
    }

    if (-1 == ::send(sock, info, sizeof(MessageInfo), 0))
    {
        perror("send");
        return -1;
    }
    close(sock);
    return 0;
}

int SocketMessage::receive(MessageInfo* info)
{
    int sock = SocketMessage::createReceiveSocket();
    if (-1 == sock)
    {
        return -1;
    }
    struct sockaddr_in whoaddr;
    socklen_t wholen = sizeof(whoaddr);
    int who;
    who = accept(sock, (struct sockaddr*)&whoaddr , &wholen);
    if (-1 == who)
    {
        perror("accept");
        return -1;
    }

    int len = recv(who, info, sizeof(MessageInfo), 0);

    if (len <= 0)
    {
        printf("something wrong? %s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        return -1;
    }
    close(who);
    close(sock);
    return 0;
}

}

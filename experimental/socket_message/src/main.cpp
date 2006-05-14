/*!
  \file   main.cpp
  \brief  socket_message

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/05/11 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "MessageList.h"

MessageList messages;

int set_myhost(struct sockaddr_in* me);
int SendMessage(int to, MessageInfo* msg)
{
    struct sockaddr_in me;
    bzero((char *)&me, sizeof(me));
    set_myhost(&me);
    me.sin_family = AF_INET;
    me.sin_port   = htons(to);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("socket()");
        return -1;
    }

    if (-1 == connect(sock, (sockaddr*)&me , sizeof(me)))
    {
        perror("connect()");
        return -1;
    }

    if (-1 == send(sock, msg, sizeof(MessageInfo), 0))
    {
        perror("send");
        return -1;
    }
    close(sock);
    return 0;
}

int CreateReceiveSocket(int port)
{
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("socket()");
        return -1;
    }

    struct sockaddr_in me;
    bzero((char *)&me, sizeof(me));
    set_myhost(&me);
    me.sin_family = AF_INET;
    me.sin_port   = htons(port);
    if (-1 == bind(sock, (struct sockaddr *)&me, sizeof(me)))
    {
        perror("bind");
        return -1;
    }

    if (-1 == (listen(sock, 5)))
    {
        perror("listen");
        return -1;
    }
    return sock;
}

int ReceiveLoop(int port)
{
    int sock = CreateReceiveSocket(port);
    if (-1 == sock) return -1;

    for (;;)
    {
        struct sockaddr_in whoaddr;
        socklen_t wholen = sizeof(whoaddr);
        int who;
        who = accept(sock, (struct sockaddr*)&whoaddr , &wholen);
        if (-1 == who)
        {
            perror("accept");
            return -1;
        }

        MessageInfo* msg = new MessageInfo;
        int len = recv(who, msg, sizeof(MessageInfo), 0);
        printf("msg receve %d:%s\n", msg->header, msg->str);
        messages.push(msg);

        if (len <= 0)
        {
            printf("something wrong? %s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            return -1;
        }
        close(who);
    }
    close(sock);
    return 0;
}

int ReceiveMessage(int port, MessageInfo* msg)
{
    return 0;
}

int SendReceiveMessage(MessageInfo* msg)
{
    return 0;
}


int server();
int client();



int main(int argc, char *argv[])
{
    MessageInfo msg;
    if (argc > 2) {
        msg.header = 1001;
        strncpy(msg.str, "hoge", 32);
        printf("msg receve %d:%s\n", msg.header, msg.str);
        SendMessage(2345, &msg);
        SendMessage(2345, &msg);

    } else {
        ReceiveLoop(2345);
    }
    return 0;
}

int set_myhost(struct sockaddr_in* me)
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

int server()
{
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("socket()");
        return -1;
    }

    struct sockaddr_in me;
    bzero((char *)&me, sizeof(me));
    set_myhost(&me);
    me.sin_family = AF_INET;
    me.sin_port   = htons(2050);
    if (-1 == bind(sock, (struct sockaddr *)&me, sizeof(me)))
    {
        perror("bind");
        return -1;
    }

    if (-1 == (listen(sock, 5)))
    {
        perror("listen");
        return -1;
    }

    for (;;)
    {
        struct sockaddr_in whoaddr;
        socklen_t wholen = sizeof(whoaddr);
        int who;
        who = accept(sock, (struct sockaddr*)&whoaddr , &wholen);
        if (-1 == who)
        {
            perror("accept");
            return -1;
        }

        char rmsg[256];
        int len = recv(who, rmsg, sizeof(rmsg), 0);
        char buf[256];
        snprintf(buf, 250, "%s", rmsg);
        printf("%s\n", buf);
        char* response = "Content-type: text/html\nStatus: 200 OK\n\nHello\n";
        send(who, response, strlen(response) + 1, 0);
        close(who);
    }
    close(sock);
    return 0;
}

int client()
{
    struct sockaddr_in me;
    bzero((char *)&me, sizeof(me));
    set_myhost(&me);
    me.sin_family = AF_INET;
    me.sin_port   = htons(2050);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("socket()");
        return -1;
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    if (-1 == connect(sock, (sockaddr*)&me , sizeof(me)))
    {
        perror("connect()");
        return -1;
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    char* msg = "GET / HTTP/1.0\nUser-Agent: Wget/1.10\nAccept: */*\nHost: 127.0.0.1:80\nConnection: Keep-Alive\n";
    if (-1 == send(sock, msg, strlen(msg), 0))
    {
        perror("send");
        return -1;
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    char rmsg[256];
    int len = recv(sock, rmsg, sizeof(rmsg), 0);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    char buf[256];
    snprintf(buf, 250, "%s", rmsg);
    printf("%s\n", buf);
    close(sock);
    return 0;
}

// int client()
// {
//     char    hostname[257];
//     char    buff[257];
//     struct  hostent *connect_host;
//     struct  sockaddr_in desthost;
//     int     s_listen;
//     int s;
//     int s_retry_counter;
//     int ret;
//     int status;
//     int     pid;
//     int portno;

//     bzero(hostname,257);
//     gethostname(hostname,256);
//     if((connect_host = gethostbyname(hostname)) == NULL) {
//         fprintf(stderr,"bad hostname!\n");
//         return -1;
//     }

//     bzero((char *)&desthost,sizeof(desthost));
//     desthost.sin_family  = AF_UNIX; // AF_UNIX in one system.
//     desthost.sin_port    = htons(3000);
//     bcopy(connect_host->h_addr, (char *)&desthost.sin_addr, connect_host->h_length);

//     if ((s_listen = socket(AF_INET, SOCK_STREAM,0)) < 0 ) {
//         fprintf(stderr,"Socket for client failed.\n");
//         return -1;
//     }

//     if (connect(s_listen, (sockaddr*)&desthost , sizeof(desthost) ) == -1) {
//         fprintf(stderr,"Connect failed.\n");
//         return -1;
//     }

//     bzero(buff,257);
//     strcpy(buff,"This program is Socket test.");
//     if ( write(s_listen, buff, strlen(buff) ) == -1 ) {
//         fprintf(stderr,"Send failed.\n");
//         return -1;
//     }

//     struct  sockaddr_in aite;
//     socklen_t aiteaddrlen;
// //     s = accept(s_listen, (struct sockaddr*)&aite , &aiteaddrlen);

// //     if (-1 == s) {
// //         printf("error\n");
// //         return -1;
// //     }

//     char rmsg[256], smsg[64];
//     int len = recv( s, rmsg, sizeof(rmsg), 0 );
//     char buf[100];
//     snprintf(buf, 256, "[%s]", rmsg);
//     printf(buf);
//     close(s_listen);

//     return 0;
// }

int server2()
{
    int s_listen;
    char hostname[257];
    struct  sockaddr_in me;
    struct  hostent *myhost;
    if ((s_listen = socket(AF_INET, SOCK_STREAM,0)) < 0 ) {
        fprintf(stderr,"Socket for server failed.\n");
        return -1;
    }
    gethostname(hostname,256);
    if((myhost = gethostbyname(hostname)) == NULL) {
        fprintf(stderr,"bad hostname!\n");
        return -1;
    }

    printf("Hostname  = %s\n", myhost->h_name);
    printf("addrtype  = %d\n", myhost->h_addrtype);
    printf("length    = %x\n", myhost->h_addr);

    memset(&me, 0, sizeof(me));
    me.sin_family  = AF_INET;
    me.sin_port    = htons(2050);
    memcpy(myhost->h_addr, (char *)&me.sin_addr, myhost->h_length);
    /*BIND*/
    if (bind(s_listen, (struct sockaddr *)&me, sizeof(me)) == -1) {
        fprintf(stderr, "Server could not bind.\n");
        return -1;
    }

    /*listen*/
    if ( (listen(s_listen, 5)) == -1) {
        fprintf(stderr, "Listen failed.\n");
        return -1;
    }
    struct  sockaddr_in aite;
    socklen_t aiteaddrlen;
    int s;
    s = accept(s_listen, (struct sockaddr*)&aite , &aiteaddrlen);

    char rmsg[64], smsg[64];
    int len = recv( s, rmsg, sizeof(rmsg), 0 );
    char buf[100];
    snprintf(buf, 64, "%s", rmsg);
    printf("%s\n", buf);

    close(s_listen);

}

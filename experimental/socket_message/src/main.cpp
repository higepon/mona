
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

int main(int argc, char *argv[])
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
    return 0;
}

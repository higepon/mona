
#include "shell.h"

#include "lwip/opt.h"

#if LWIP_SOCKET

#include "lwip/sockets.h"
#include "lwip/sys.h"

#include <string.h>

#ifndef SOCK_TARGET_HOST
#define SOCK_TARGET_HOST  "192.168.1.1"
#endif

#ifndef SOCK_TARGET_PORT
#define SOCK_TARGET_PORT  80
#endif

/** This is an example function that tests
    blocking- and nonblocking connect. */
static void
sockex_nonblocking_connect(void *arg)
{
  int s;
  int ret;
  u32_t opt;
  struct sockaddr_in addr;
  fd_set readset;
  fd_set writeset;
  fd_set errset;
  struct timeval tv;
  u32_t ticks_a, ticks_b;

  LWIP_UNUSED_ARG(arg);
  /* set up address to connect to */
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SOCK_TARGET_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  /* first try blocking: */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should succeed */
  LWIP_ASSERT("ret == 0", ret == 0);

  /* write something */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == 4", ret == 4);

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);


  /* now try nonblocking:
     this test only works if it is fast enough, i.e. no breakpoints, please! */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* nonblocking */
  opt = 1;
  ret = lwip_ioctl(s, FIONBIO, &opt);
  LWIP_ASSERT("ret == 0", ret == 0);

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should have an error: "inprogress" */
  LWIP_ASSERT("ret == -1", ret == -1);
  LWIP_ASSERT("errno == EINPROGRESS", errno == EINPROGRESS);

  /* write should fail, too */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == -1", ret == -1);
  LWIP_ASSERT("errno == EINPROGRESS", errno == EINPROGRESS);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  /* select without waiting should fail */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, &tv);
  LWIP_ASSERT("ret == 0", ret == 0);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  ticks_a = 0; //;sys_now();
  /* select with waiting should succeed */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, NULL);
  ticks_b = 0; //sys_now();
  LWIP_ASSERT("ret == 1", ret == 1);
  LWIP_ASSERT("FD_ISSET(s, &writeset)", FD_ISSET(s, &writeset));
  LWIP_ASSERT("!FD_ISSET(s, &readset)", !FD_ISSET(s, &readset));
  LWIP_ASSERT("!FD_ISSET(s, &errset)", !FD_ISSET(s, &errset));

  /* now write should succeed */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == 4", ret == 4);

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);

  printf("select() needed %d ticks to return writable\n", ticks_b - ticks_a);


  /* now try nonblocking to invalid address:
     this test only works if it is fast enough, i.e. no breakpoints, please! */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* nonblocking */
  opt = 1;
  ret = lwip_ioctl(s, FIONBIO, &opt);
  LWIP_ASSERT("ret == 0", ret == 0);

  addr.sin_addr.s_addr++;

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should have an error: "inprogress" */
  LWIP_ASSERT("ret == -1", ret == -1);
  LWIP_ASSERT("errno == EINPROGRESS", errno == EINPROGRESS);

  /* write should fail, too */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == -1", ret == -1);
  LWIP_ASSERT("errno == EINPROGRESS", errno == EINPROGRESS);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  /* select without waiting should fail */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, &tv);
  LWIP_ASSERT("ret == 0", ret == 0);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  ticks_a = 0; //sys_now();
  /* select with waiting should eventually succeed and return errset! */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, NULL);
  ticks_b = 0;//sys_now();
  LWIP_ASSERT("ret > 0", ret > 0);
  LWIP_ASSERT("!FD_ISSET(s, &writeset)", !FD_ISSET(s, &writeset));
  LWIP_ASSERT("!FD_ISSET(s, &readset)", !FD_ISSET(s, &readset));
  LWIP_ASSERT("FD_ISSET(s, &errset)", FD_ISSET(s, &errset));

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);

  printf("select() needed %d ticks to return error\n", ticks_b - ticks_a);
  printf("all tests done, thread ending\n");
}

/** This is an example function that tests
    the recv function (timeout etc.). */

#define BUF_SIZE 256
static void
sockex_testrecv(void *arg)
{
  int sock;
  struct sockaddr_in addr;
  char *request = "GET / HTTP/1.0¥r¥n¥r¥n";
  char buf[BUF_SIZE];
  int read_size;
  int i;
  
  if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    fprintf(stderr, "could not create socket¥n");
    exit(EXIT_FAILURE);
  }
  
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr("192.168.1.12");
  addr.sin_port = htons(4649);
  
  if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    logprintf("could not connect to remote host¥n");
    exit(EXIT_FAILURE);
  }
  
  if(send(sock, request, strlen(request), 0) < 0){
    logprintf("could not send message : %s¥n", request);
    exit(EXIT_FAILURE);
  }
  logprintf("higehigehige");
  while((read_size = recv(sock, buf, 127, 0)) > 0){
    logprintf("<1>");
    for (i = 0; i < 10; i++) {
      logprintf("<%c>", buf[i]);
    }
  }
  logprintf("hige=%d\n", read_size);

  
  close(sock);
  
  exit(EXIT_SUCCESS);
}

void socket_examples_init(void)
{
  //  sys_thread_new("sockex_nonblocking_connect", sockex_nonblocking_connect, NULL, 0, 0);
  sys_thread_new("sockex_testrecv", sockex_testrecv, NULL, 0, 0);
}

#endif /* LWIP_SOCKETS */

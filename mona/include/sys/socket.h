/*
 * socket.h - BSD Socket
 *
 *   Copyright (c) 2010 Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef SYS_SOCKET_
#define SYS_SOCKET_

#include <sys/types.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
#ifndef socklen_t
#  define socklen_t u32_t
#endif

struct sockaddr {
  u8_t sa_len;
  u8_t sa_family;
  char sa_data[14];
};

/* Socket protocol types (TCP/UDP/RAW) */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_UDPLITE 136

#define INADDR_NONE         ((u32_t)0xffffffffUL)  /* 255.255.255.255 */
#define INADDR_LOOPBACK     ((u32_t)0x7f000001UL)  /* 127.0.0.1 */
#define INADDR_ANY          ((u32_t)0x00000000UL)  /* 0.0.0.0 */
#define INADDR_BROADCAST    ((u32_t)0xffffffffUL)  /* 255.255.255.255 */

/*
 * Commands for ioctlsocket(),  taken from the BSD file fcntl.h.
 * lwip_ioctl only supports FIONREAD and FIONBIO, for now
 *
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 2 bits of the upper word are used
 * to encode the in/out status of the parameter; for now
 * we restrict parameters to at most 128 bytes.
 */
#if !defined(FIONREAD) || !defined(FIONBIO)
#define IOCPARM_MASK    0x7fU           /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000UL    /* no parameters */
#define IOC_OUT         0x40000000UL    /* copy out parameters */
#define IOC_IN          0x80000000UL    /* copy in parameters */
#define IOC_INOUT       (IOC_IN|IOC_OUT)
                                        /* 0x20000000 distinguishes new &
                                           old ioctl's */
#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))

#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#endif /* !defined(FIONREAD) || !defined(FIONBIO) */

#ifndef FIONREAD
#define FIONREAD    _IOR('f', 127, unsigned long) /* get # bytes to read */
#endif
#ifndef FIONBIO
#define FIONBIO     _IOW('f', 126, unsigned long) /* set/clear non-blocking i/o */
#endif

/* For compatibility with BSD code */
struct in_addr {
  u32_t s_addr;
};

/* members are in network byte order */
struct sockaddr_in {
  u8_t sin_len;
  u8_t sin_family;
  u16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};

uint16_t htons(uint16_t n);
uint16_t ntohs(uint16_t n);
uint32_t htonl(uint32_t n);
uint32_t ntohl(uint32_t n);

/* MEMP_NUM_NETCONN: the number of struct netconns. */
#define MEMP_NUM_NETCONN        10
#define FD_SETSIZE    MEMP_NUM_NETCONN
#define FD_SET(n, p)  ((p)->fd_bits[(n)/8] |=  (1 << ((n) & 7)))
#define FD_CLR(n, p)  ((p)->fd_bits[(n)/8] &= ~(1 << ((n) & 7)))
#define FD_ISSET(n,p) ((p)->fd_bits[(n)/8] &   (1 << ((n) & 7)))
#define FD_ZERO(p)    memset((void*)(p),0,sizeof(*(p)))
typedef struct fd_set {
    unsigned char fd_bits [(FD_SETSIZE+7)/8];
} fd_set;


// -recvfrom
// -sendto

/*
    Title: Socket
    #include <sys/socket.h>

    Though lwip supports getsockname, getpeername, recvfrom and sendto, currently they are not supported on Mona.
*/
/*
   function: send

   Send

   Parameters:

     sockfd - socket
     buf - buffer
     len - buffer size
     flags - flags

   Returns:
     Returns positive integer, sent size if successfully sent. otherwise returns -1 and errno is set.

*/
int send(int sockfd, void* buf, size_t len, int flags);

/*
   function: recv

   Receive

   Parameters:

     sockfd - socket
     buf - buffer
     len - buffer size
     flags - flags

   Returns:
     Returns positive integer if successfully received. otherwise returns -1 and errno is set.

*/
int recv(int sockfd, void* buf, size_t len, int flags);


/*
   function: socket

   Creates socket

   Parameters:

     domain - domain
     type - type
     protocol - protocol

   Returns:
     Returns socket if successfully created. otherwise returns -1 and errno is set.

*/
int socket(int domain, int type, int protocol);

/*
   function: connect

   connect

   Parameters:

     sockfd - socket
     addr - address
     addrlen - length of address

   Returns:
     Returns 0 if successfully sent. otherwise returns -1 and errno is set.

*/
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/*
   function: closesocket

   Close socket

   Parameters:

     sockfd - socket

   Returns:
     Returns 0 if successfully sent. otherwise returns -1 and errno is set.

*/
int closesocket(int sockfd);

/*
   function: setsockopt

   Set socket option

   Parameters:

     sockfd - socket

   Returns:
     Returns 0 if successfully closed. otherwise returns -1 and errno is set.

*/
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

/*
   function: getsockopt

   Get socket option

   Parameters:

     sockfd - socket

   Returns:
     Returns 0 if successfully sent. otherwise returns -1 and errno is set.

*/
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

/*
   function: shutdown

   shutdown the socket

   Parameters:

     sockfd - socket
     how - <<SHUT_RD>>, <<SHUT_WR>> or SHUT_RDWR.

   Returns:
     Returns 0 if successfully shutdowned. otherwise returns -1 and errno is set.

*/
int shutdown(int sockfd, int how);

/*
   function: listen

   Listen on the socket

   Parameters:

     sockfd - socket
     backlog - backlog

   Returns:
     Returns 0 if successfully shutdowned. otherwise returns -1 and errno is set.

*/
int listen(int sockfd, int backlog);

/*
   function: bind

   bind

   Returns:
     Returns 0 if successfully shutdowned. otherwise returns -1 and errno is set.

*/
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/*
   function: accept

   accept on socket

   Returns:
     Returns 0 if successfully shutdowned. otherwise returns -1 and errno is set.

*/
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);

/*
   function: select

   exceptfds parameter is not supported.

   Returns:
     Returns 0 if successfully shutdowned. otherwise returns -1 and errno is set.

*/
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);

/*
   function: ioctlsocket

   ioctl for socket

   Parameters:

     sockfd - socket
     cmd - <<FIONREAD>> or <<FIONBIO>>.
     argp - argument

   Returns:
     Returns 0 if successfully shutdowned. otherwise returns -1 and errno is set.

*/
int ioctlsocket(int sockfd, long cmd, void *argp);

int getpeername(int socket, struct sockaddr* address, socklen_t* address_len);
int getsockname(int socket, struct sockaddr* address, socklen_t* address_len);

#define SHUT_RD 0
#define  SHUT_WR 1
#define SHUT_RDWR 2

/* copied from lwip/sockets.h */
/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define  SOL_SOCKET  0xfff    /* options for socket level */

/*
 * Option flags per-socket. These must match the SOF_ flags in ip.h!
 */
#define  SO_DEBUG       0x0001 /* Unimplemented: turn on debugging info recording */
#define  SO_ACCEPTCONN  0x0002 /* socket has had listen() */
#define  SO_REUSEADDR   0x0004 /* Unimplemented: allow local address reuse */
#define  SO_KEEPALIVE   0x0008 /* keep connections alive */
#define  SO_DONTROUTE   0x0010 /* Unimplemented: just use interface addresses */
#define  SO_BROADCAST   0x0020 /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
#define  SO_USELOOPBACK 0x0040 /* Unimplemented: bypass hardware when possible */
#define  SO_LINGER      0x0080 /* linger on close if data present */
#define  SO_OOBINLINE   0x0100 /* Unimplemented: leave received OOB data in line */
#define  SO_REUSEPORT   0x0200 /* Unimplemented: allow local address & port reuse */

#define SO_DONTLINGER   ((int)(~SO_LINGER))

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF    0x1001    /* Unimplemented: send buffer size */
#define SO_RCVBUF    0x1002    /* receive buffer size */
#define SO_SNDLOWAT  0x1003    /* Unimplemented: send low-water mark */
#define SO_RCVLOWAT  0x1004    /* Unimplemented: receive low-water mark */
#define SO_SNDTIMEO  0x1005    /* Unimplemented: send timeout */
#define SO_RCVTIMEO  0x1006    /* receive timeout */
#define SO_ERROR     0x1007    /* get error status and clear */
#define SO_TYPE      0x1008    /* get socket type */
#define SO_CONTIMEO  0x1009    /* Unimplemented: connect timeout */
#define SO_NO_CHECK  0x100a    /* don't create UDP checksum */



#ifdef __cplusplus
}
#endif

#endif /* SYS_SOCKET_*/

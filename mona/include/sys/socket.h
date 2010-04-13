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

struct timeval {
  long    tv_sec;         /* seconds */
  long    tv_usec;        /* and microseconds */
};


// -[done]accept
// -[done]bind
// -[done]shutdown
// -[done]closesocket
// -[done]connect
// -getsockname
// -getpeername
// -[done]setsockopt
// -[done]getsockopt
// -[done]listen
// -[done]recv
// -recvfrom
// -[done]send
// -sendto
// -[done]socket
// -[done]select
// -[done]ioctlsocket

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

/* copied from lwip/err.h */
#define  EPERM     1  /* Operation not permitted */
#define  ENOENT     2  /* No such file or directory */
#define  ESRCH     3  /* No such process */
#define  EINTR     4  /* Interrupted system call */
#define  EIO     5  /* I/O error */
#define  ENXIO     6  /* No such device or address */
#define  E2BIG     7  /* Arg list too long */
#define  ENOEXEC     8  /* Exec format error */
#define  EBADF     9  /* Bad file number */
#define  ECHILD    10  /* No child processes */
#define  EAGAIN    11  /* Try again */
#define  ENOMEM    12  /* Out of memory */
#define  EACCES    13  /* Permission denied */
#define  EFAULT    14  /* Bad address */
#define  ENOTBLK    15  /* Block device required */
#define  EBUSY    16  /* Device or resource busy */
#define  EEXIST    17  /* File exists */
#define  EXDEV    18  /* Cross-device link */
#define  ENODEV    19  /* No such device */
#define  ENOTDIR    20  /* Not a directory */
#define  EISDIR    21  /* Is a directory */
#define  EINVAL    22  /* Invalid argument */
#define  ENFILE    23  /* File table overflow */
#define  EMFILE    24  /* Too many open files */
#define  ENOTTY    25  /* Not a typewriter */
#define  ETXTBSY    26  /* Text file busy */
#define  EFBIG    27  /* File too large */
#define  ENOSPC    28  /* No space left on device */
#define  ESPIPE    29  /* Illegal seek */
#define  EROFS    30  /* Read-only file system */
#define  EMLINK    31  /* Too many links */
#define  EPIPE    32  /* Broken pipe */
#define  EDOM    33  /* Math argument out of domain of func */
#define  ERANGE    34  /* Math result not representable */
#define  EDEADLK    35  /* Resource deadlock would occur */
#define  ENAMETOOLONG  36  /* File name too long */
#define  ENOLCK    37  /* No record locks available */
#define  ENOSYS    38  /* Function not implemented */
#define  ENOTEMPTY  39  /* Directory not empty */
#define  ELOOP    40  /* Too many symbolic links encountered */
#define  EWOULDBLOCK  EAGAIN  /* Operation would block */
#define  ENOMSG    42  /* No message of desired type */
#define  EIDRM    43  /* Identifier removed */
#define  ECHRNG    44  /* Channel number out of range */
#define  EL2NSYNC  45  /* Level 2 not synchronized */
#define  EL3HLT    46  /* Level 3 halted */
#define  EL3RST    47  /* Level 3 reset */
#define  ELNRNG    48  /* Link number out of range */
#define  EUNATCH    49  /* Protocol driver not attached */
#define  ENOCSI    50  /* No CSI structure available */
#define  EL2HLT    51  /* Level 2 halted */
#define  EBADE    52  /* Invalid exchange */
#define  EBADR    53  /* Invalid request descriptor */
#define  EXFULL    54  /* Exchange full */
#define  ENOANO    55  /* No anode */
#define  EBADRQC    56  /* Invalid request code */
#define  EBADSLT    57  /* Invalid slot */

#define  EDEADLOCK  EDEADLK

#define  EBFONT    59  /* Bad font file format */
#define  ENOSTR    60  /* Device not a stream */
#define  ENODATA    61  /* No data available */
#define  ETIME    62  /* Timer expired */
#define  ENOSR    63  /* Out of streams resources */
#define  ENONET    64  /* Machine is not on the network */
#define  ENOPKG    65  /* Package not installed */
#define  EREMOTE    66  /* Object is remote */
#define  ENOLINK    67  /* Link has been severed */
#define  EADV    68  /* Advertise error */
#define  ESRMNT    69  /* Srmount error */
#define  ECOMM    70  /* Communication error on send */
#define  EPROTO    71  /* Protocol error */
#define  EMULTIHOP  72  /* Multihop attempted */
#define  EDOTDOT    73  /* RFS specific error */
#define  EBADMSG    74  /* Not a data message */
#define  EOVERFLOW  75  /* Value too large for defined data type */
#define  ENOTUNIQ  76  /* Name not unique on network */
#define  EBADFD    77  /* File descriptor in bad state */
#define  EREMCHG    78  /* Remote address changed */
#define  ELIBACC    79  /* Can not access a needed shared library */
#define  ELIBBAD    80  /* Accessing a corrupted shared library */
#define  ELIBSCN    81  /* .lib section in a.out corrupted */
#define  ELIBMAX    82  /* Attempting to link in too many shared libraries */
#define  ELIBEXEC  83  /* Cannot exec a shared library directly */
#define  EILSEQ    84  /* Illegal byte sequence */
#define  ERESTART  85  /* Interrupted system call should be restarted */
#define  ESTRPIPE  86  /* Streams pipe error */
#define  EUSERS    87  /* Too many users */
#define  ENOTSOCK  88  /* Socket operation on non-socket */
#define  EDESTADDRREQ  89  /* Destination address required */
#define  EMSGSIZE  90  /* Message too long */
#define  EPROTOTYPE  91  /* Protocol wrong type for socket */
#define  ENOPROTOOPT  92  /* Protocol not available */
#define  EPROTONOSUPPORT  93  /* Protocol not supported */
#define  ESOCKTNOSUPPORT  94  /* Socket type not supported */
#define  EOPNOTSUPP  95  /* Operation not supported on transport endpoint */
#define  EPFNOSUPPORT  96  /* Protocol family not supported */
#define  EAFNOSUPPORT  97  /* Address family not supported by protocol */
#define  EADDRINUSE  98  /* Address already in use */
#define  EADDRNOTAVAIL  99  /* Cannot assign requested address */
#define  ENETDOWN  100  /* Network is down */
#define  ENETUNREACH  101  /* Network is unreachable */
#define  ENETRESET  102  /* Network dropped connection because of reset */
#define  ECONNABORTED  103  /* Software caused connection abort */
#define  ECONNRESET  104  /* Connection reset by peer */
#define  ENOBUFS    105  /* No buffer space available */
#define  EISCONN    106  /* Transport endpoint is already connected */
#define  ENOTCONN  107  /* Transport endpoint is not connected */
#define  ESHUTDOWN  108  /* Cannot send after transport endpoint shutdown */
#define  ETOOMANYREFS  109  /* Too many references: cannot splice */
#define  ETIMEDOUT  110  /* Connection timed out */
#define  ECONNREFUSED  111  /* Connection refused */
#define  EHOSTDOWN  112  /* Host is down */
#define  EHOSTUNREACH  113  /* No route to host */
#define  EALREADY  114  /* Operation already in progress */
#define  EINPROGRESS  115  /* Operation now in progress */
#define  ESTALE    116  /* Stale NFS file handle */
#define  EUCLEAN    117  /* Structure needs cleaning */
#define  ENOTNAM    118  /* Not a XENIX named type file */
#define  ENAVAIL    119  /* No XENIX semaphores available */
#define  EISNAM    120  /* Is a named type file */
#define  EREMOTEIO  121  /* Remote I/O error */
#define  EDQUOT    122  /* Quota exceeded */

#define  ENOMEDIUM  123  /* No medium found */
#define  EMEDIUMTYPE  124  /* Wrong medium type */


#define ENSROK    0 /* DNS server returned answer with no data */
#define ENSRNODATA  160 /* DNS server returned answer with no data */
#define ENSRFORMERR 161 /* DNS server claims query was misformatted */
#define ENSRSERVFAIL 162  /* DNS server returned general failure */
#define ENSRNOTFOUND 163  /* Domain name not found */
#define ENSRNOTIMP  164 /* DNS server does not implement requested operation */
#define ENSRREFUSED 165 /* DNS server refused query */
#define ENSRBADQUERY 166  /* Misformatted DNS query */
#define ENSRBADNAME 167 /* Misformatted domain name */
#define ENSRBADFAMILY 168 /* Unsupported address family */
#define ENSRBADRESP 169 /* Misformatted DNS reply */
#define ENSRCONNREFUSED 170 /* Could not contact DNS servers */
#define ENSRTIMEOUT 171 /* Timeout while contacting DNS servers */
#define ENSROF    172 /* End of file */
#define ENSRFILE  173 /* Error reading file */
#define ENSRNOMEM 174 /* Out of memory */
#define ENSRDESTRUCTION 175 /* Application terminated lookup */
#define ENSRQUERYDOMAINTOOLONG  176 /* Domain name is too long */
#define ENSRCNAMELOOP 177 /* Domain name is too long */


#ifdef __cplusplus
}
#endif

#endif /* SYS_SOCKET_*/

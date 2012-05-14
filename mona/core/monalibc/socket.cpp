/*
 * socket.cpp - BSD socket
 *
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
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

#include <sys/socket.h>
#include <netdb.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <servers/net.h>
#include <monalibc/errno.h>
#include <time.h>

#if 0
#define SOCKET_LOG() { uint64_t x = syscall_now_in_nanosec() / 1000000; _logprintf("[socket] %s %s:%d %d\n", __func__, __FILE__, __LINE__, (unsigned int)x);}
#define SOCKET_LOGF(...) { uint64_t x = syscall_now_in_nanosec() / 1000000; _logprintf("[socket] %s %s:%d %d", __func__, __FILE__, __LINE__, (unsigned int)x), _logprintf(__VA_ARGS__); }
#else
#define SOCKET_LOG()
#define SOCKET_LOGF(...)
#endif

using namespace MonAPI;

typedef struct GetaddrinfoPacket {
    uintptr_t nodeLen;
    struct addrinfo hints;
    char data[0];
} GetaddrinfoPacket;

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res)
{
    // construct addrinfo to struct
    uintptr_t nodeLen = node == NULL ? 0 : strlen(node) + 1;
    uintptr_t serviceLen = service == NULL ? 0 : strlen(service) + 1;
    uintptr_t structSize = sizeof(GetaddrinfoPacket) + nodeLen + serviceLen;
    uint8_t* p = new uint8_t[structSize];
    GetaddrinfoPacket* pack = (GetaddrinfoPacket*)p;
    pack->nodeLen = nodeLen;
    pack->hints = *hints;
    if (node != NULL) {
        strcpy(pack->data, node);
    }
    if (service != NULL) {
        strcpy(pack->data + nodeLen, service);
    }

    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return M_NAME_NOT_FOUND;
    }
    if (Message::send(id, MSG_NET_GET_ADDR_INFO) != M_OK) {
        return EBADF;
    }
    if (Message::sendBuffer(id, p, structSize) != M_OK) {
        return EBADF;
    }
    BufferReceiver* receiver = Message::receiveBuffer(id);
    if (receiver->bufferSize() != 0) {
        struct addrinfo* ainfo = new struct addrinfo;
        memcpy(ainfo, receiver->buffer(), sizeof(struct addrinfo));
        ainfo->ai_addr = (struct sockaddr*)(new uint8_t[ainfo->ai_addrlen]);
        memcpy(ainfo->ai_addr, receiver->buffer() + sizeof(struct addrinfo), ainfo->ai_addrlen);
        MONA_ASSERT(receiver->bufferSize() == (sizeof(struct addrinfo) + ainfo->ai_addrlen));
        *res = ainfo;
    }
    delete receiver;
    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_GET_ADDR_INFO;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    return dst.arg2;
}

void freeaddrinfo(struct addrinfo *res)
{
    delete res;
}

int connect(int sockfd, const struct sockaddr* name, socklen_t namelen)
{
    SOCKET_LOGF("sock=%d\n", sockfd);
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    if (Message::send(id, MSG_NET_SOCKET_CONN, sockfd, namelen) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    if (Message::sendBuffer(id, name, namelen) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_CONN;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }
    errno = dst.arg3;
    SOCKET_LOGF("sockfd=%d ret=%d\n", sockfd, dst.arg2);
    return dst.arg2;
}

int socket(int domain, int type, int protocol)
{
    SOCKET_LOG();
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    MessageInfo ret;
    if (Message::sendReceive(&ret, id, MSG_NET_SOCKET_SOCK, domain, type, protocol) != M_OK) {
        SOCKET_LOG();
        return EINVAL;
    }
    errno = ret.arg3;
    SOCKET_LOGF("ret=%d\n", ret.arg2);
    return ret.arg2;
}

int closesocket(int sockfd)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    MessageInfo ret;
    if (Message::sendReceive(&ret, id, MSG_NET_SOCKET_CLOSE, sockfd) != M_OK) {
        return EINVAL;
    }
    return ret.arg2;
}

int send(int sockfd, void* buf, size_t len, int flags)
{
    uint32_t id ;
    SOCKET_LOGF("sockfd=%d\n", sockfd);
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }
    if (Message::send(id, MSG_NET_SOCKET_SEND, sockfd, len, flags) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    if (Message::sendBuffer(id, buf, len) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_SEND;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }
    errno = dst.arg3;
    SOCKET_LOGF("sockfd=%d ret=%d\n", sockfd, dst.arg2);
    return dst.arg2;
}

int recv(int sockfd, void* buf, size_t len, int flags)
{
    SOCKET_LOGF("sockfd=%d\n", sockfd);
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }

    SharedMemory* shm = new SharedMemory(len);
    shm->map(true);
    uint32_t param[2];
    param[0] = shm->handle();
    param[1] = shm->size();
    if (Message::send(id, MSG_NET_SOCKET_RECV, sockfd, len, flags, (char*)param) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }
    SOCKET_LOGF("sockfd=%d\n", sockfd);
    // BufferReceiver* receiver = Message::receiveBuffer(id);
    // if (receiver->bufferSize() > 0) {
    //     memcpy(buf, receiver->buffer(), receiver->bufferSize());
    // }
    SOCKET_LOGF("sockfd=%d\n", sockfd);
    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_RECV;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }
    if (dst.arg2 > 0) {
      memcpy(buf, shm->data(), dst.arg2);
    }
    delete shm;
    errno = dst.arg3;
    SOCKET_LOGF("sockfd=%d ret=%d\n", sockfd, dst.arg2);
    return dst.arg2;
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    if (Message::send(id, MSG_NET_SOCKET_SET_OPTION, sockfd, level, optname) != M_OK) {
        return EBADF;
    }

    if (Message::sendBuffer(id, optval, optlen) != M_OK) {
        return EBADF;
    }

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_SET_OPTION;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    char buf[sizeof(socklen_t)];
    memcpy(buf, optlen, sizeof(socklen_t));
    if (Message::send(id, MSG_NET_SOCKET_GET_OPTION, sockfd, level, optname, buf) != M_OK) {
        return EBADF;
    }

    BufferReceiver* receiver = Message::receiveBuffer(id);
    *optlen = receiver->bufferSize();
    memcpy(optval, receiver->buffer(), *optlen);
    delete receiver;

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_GET_OPTION;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}

int shutdown(int sockfd, int how)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    MessageInfo ret;
    if (Message::sendReceive(&ret, id, MSG_NET_SOCKET_SHUTDOWN, sockfd, how) != M_OK) {
        return EINVAL;
    }
    errno = ret.arg3;
    return ret.arg2;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    if (Message::send(id, MSG_NET_SOCKET_BIND, sockfd, addrlen) != M_OK) {
        return EBADF;
    }

    if (Message::sendBuffer(id, addr, addrlen) != M_OK) {
        return EBADF;
    }

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_BIND;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}

int listen(int sockfd, int backlog)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    MessageInfo ret;
    if (Message::sendReceive(&ret, id, MSG_NET_SOCKET_LISTEN, sockfd, backlog) != M_OK) {
        return EINVAL;
    }
    errno = ret.arg3;
    return ret.arg2;
}

int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    MessageInfo msg;
    if (Message::sendReceive(&msg, id, MSG_NET_SOCKET_ACCEPT, sockfd) != M_OK) {
        return EBADF;
    }

    uintptr_t waitId = msg.arg2;

    BufferReceiver* receiver = Message::receiveBuffer(waitId);
    *addrlen = receiver->bufferSize();
    if (receiver->bufferSize() != 0) {
        memcpy(addr, receiver->buffer(), (*addrlen) > sizeof(sockaddr) ? sizeof(sockaddr) : *addrlen);
    }
    delete receiver;
    MessageInfo src;
    MessageInfo dst;
    src.from = waitId;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_SOCKET_ACCEPT;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}


int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout)
{
    SOCKET_LOG();
    MONA_ASSERT((4 + sizeof(fd_set) * 3 + sizeof(struct timeval)) <= MESSAGE_INFO_MAX_STR_LENGTH);
    uint8_t buf[MESSAGE_INFO_MAX_STR_LENGTH];

    bool haveReadFds = readfds != NULL;
    bool haveWritefds =  writefds != NULL;
    bool haveExceptFds =  exceptfds != NULL;
    bool haveTimeout =  timeout != NULL;

    buf[0] = haveReadFds;
    buf[1] = haveWritefds;
    buf[2] = haveExceptFds;
    buf[3] = haveTimeout;

    if (haveReadFds) {
        SOCKET_LOG();
        memcpy(buf + 4, readfds, sizeof(fd_set));
    }
    if (haveWritefds) {
        SOCKET_LOG();
        memcpy(buf + 4 + sizeof(fd_set), writefds, sizeof(fd_set));
    }
    if (haveExceptFds) {
        SOCKET_LOG();
        memcpy(buf + 4 + sizeof(fd_set) * 2, exceptfds, sizeof(fd_set));
    }
    if (haveTimeout) {
        SOCKET_LOG();
        memcpy(buf + 4 + sizeof(fd_set) * 3, timeout, sizeof(struct timeval));
    }

    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        SOCKET_LOG();
        return EBADF;
    }
    MessageInfo ret;
    if (Message::sendReceive(&ret, id, MSG_NET_SELECT, nfds, 0, 0, (char*)buf) != M_OK) {
        SOCKET_LOG();
        return EINVAL;
    }
    if (haveReadFds) {
        SOCKET_LOG();
        memcpy(readfds, ret.str, sizeof(fd_set));
    }
    if (haveWritefds) {
        SOCKET_LOG();
        memcpy(writefds, ret.str + sizeof(fd_set), sizeof(fd_set));
    }
    if (haveExceptFds) {
        SOCKET_LOG();
        memcpy(exceptfds, ret.str + sizeof(fd_set) * 2, sizeof(fd_set));
    }
    if (haveTimeout) {
        SOCKET_LOG();
        memcpy(timeout, ret.str + sizeof(fd_set) * 3, sizeof(struct timeval));
    }
    errno = ret.arg3;
    return ret.arg2;
}

int ioctlsocket(int sockfd, long cmd, void *argp)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    MessageInfo ret;
    uint8_t buf[MESSAGE_INFO_MAX_STR_LENGTH];
    if (argp != NULL) {
        *((uint32_t*)buf) = *((uint32_t*)argp); // for lwip uint32_t is enough
    }

    if (Message::sendReceive(&ret, id, MSG_NET_SOCKET_CONTROL, sockfd, cmd, argp != NULL, (char*)buf) != M_OK) {
        return EINVAL;
    }

    if (argp != NULL) {
        *((uint32_t*)argp) = *((uint32_t*)buf);
    }
    errno = ret.arg3;
    SOCKET_LOGF("ret=%d\n", ret.arg2);
    return ret.arg2;
}

int getpeername(int sockfd, struct sockaddr* address, socklen_t* address_len)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    char buf[sizeof(socklen_t)];
    memcpy(buf, address_len, sizeof(socklen_t));
    if (Message::send(id, MSG_NET_GET_PEERNAME, sockfd, 0, 0, buf) != M_OK) {
        return EBADF;
    }

    BufferReceiver* receiver = Message::receiveBuffer(id);
    *address_len = receiver->bufferSize();
    memcpy(address, receiver->buffer(), *address_len);
    delete receiver;

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_GET_PEERNAME;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}

int getsockname(int sockfd, struct sockaddr* address, socklen_t* address_len)
{
    uint32_t id ;
    if (monapi_name_whereis("/servers/net", id) != M_OK) {
        return EBADF;
    }
    char buf[sizeof(socklen_t)];
    memcpy(buf, address_len, sizeof(socklen_t));
    if (Message::send(id, MSG_NET_GET_SOCKNAME, sockfd, 0, 0, buf) != M_OK) {
        return EBADF;
    }

    BufferReceiver* receiver = Message::receiveBuffer(id);
    *address_len = receiver->bufferSize();
    memcpy(address, receiver->buffer(), *address_len);
    delete receiver;

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_OK;
    src.arg1 = MSG_NET_GET_SOCKNAME;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}


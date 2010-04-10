/*
 * messageLoop.cpp - message loop which offers BSD socket using MonAPI Message.
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

#include "monapi.h"
#include "lwip/opt.h"
#include "lwip/sockets.h"
extern "C" {
#include "lwip/netdb.h"
}
#include "lwip/sys.h"
#include "messageLoop.h"
#include <servers/net.h>

using namespace MonAPI;

typedef struct GetaddrinfoPacket {
    uintptr_t nodeLen;
    struct addrinfo hints;
    char data[0];
} GetaddrinfoPacket;

static void __fastcall messageLoop(void* arg)
{
    if (monapi_notify_server_start("MONITOR.BIN") != M_OK) {
        MONAPI_WARN("net server can't notify to MONITOR");
        exit(-1);
    }

    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        switch (msg.header) {
        case MSG_NET_SOCKET_CONN:
        {
            int sockfd = msg.arg1;
            socklen_t namelen = msg.arg2;
            BufferReceiver* receiver = Message::receiveBuffer(msg.from);
            int ret = connect(sockfd, (struct sockaddr*)receiver->buffer(), namelen);
            delete receiver;

            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_BIND:
        {
            int sockfd = msg.arg1;
            socklen_t addrlen = msg.arg2;
            BufferReceiver* receiver = Message::receiveBuffer(msg.from);
            int ret = bind(sockfd, (struct sockaddr*)receiver->buffer(), addrlen);
            delete receiver;
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_SOCK:
        {
            int domain = msg.arg1;
            int type = msg.arg2;
            int protocol = msg.arg3;
            int ret = socket(domain, type, protocol);
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_LISTEN:
        {
            int sockfd = msg.arg1;
            int backlog = msg.arg2;
            int ret = listen(sockfd, backlog);
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_SHUTDOWN:
        {
            int sockfd = msg.arg1;
            int how = msg.arg2;
            int ret = shutdown(sockfd, how);
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_CLOSE:
        {
            int sockfd = msg.arg1;
            int ret = close(sockfd);
            if (Message::reply(&msg, ret) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_SEND:
        {
            int sockfd = msg.arg1;
            size_t len = msg.arg2;
            int flags = msg.arg3;
            BufferReceiver* receiver = Message::receiveBuffer(msg.from);
            int ret = send(sockfd, receiver->buffer(), len, flags);
            delete receiver;
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_SET_OPTION:
        {
            int sockfd = msg.arg1;
            int level = msg.arg2;
            int optname = msg.arg3;
            BufferReceiver* receiver = Message::receiveBuffer(msg.from);
            void* optval = (void*)receiver->buffer();
            int optlen = receiver->bufferSize();
            int ret = setsockopt(sockfd, level, optname, optval, optlen);
            delete receiver;
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_ACCEPT:
        {
            int sockfd = msg.arg1;
            struct sockaddr_in waddr;
            socklen_t writer_len;
            int s = accept(sockfd, (struct sockaddr*)&waddr, &writer_len);
            if (Message::sendBuffer(msg.from, &waddr, writer_len) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            if (Message::reply(&msg, s, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_GET_OPTION:
        {
            int sockfd = msg.arg1;
            int level = msg.arg2;
            int optname = msg.arg3;
            socklen_t optlen = *((socklen_t*)&msg.str);
            uint8_t* buf = new uint8_t[optlen];
            int ret = getsockopt(sockfd, level, optname, buf, &optlen);
            if (Message::sendBuffer(msg.from, buf, optlen) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            delete[] buf;
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_RECV:
        {
            static uint8_t* buffer = NULL;
            static uintptr_t bufferSize = 127;
            if (NULL == buffer) {
                buffer = new uint8_t[bufferSize];
            }

            int sockfd = msg.arg1;
            size_t len = msg.arg2;
            int flags = msg.arg3;

            if (len > bufferSize) {
                if (buffer != NULL) {
                    delete[] buffer;
                }
                bufferSize = len;
                buffer = new uint8_t[bufferSize];
            }

            int ret = recv(sockfd, buffer, len, flags);

            if (Message::sendBuffer(msg.from, buffer, ret > 0 ? ret : 0) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }

            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_GET_ADDR_INFO:
        {
            BufferReceiver* receiver = Message::receiveBuffer(msg.from);
            if (receiver->bufferSize() <= sizeof(GetaddrinfoPacket)) {
                MONAPI_WARN("getaddrinfoPacket is too small");
                break;
            }
            GetaddrinfoPacket* packet = (GetaddrinfoPacket*)receiver->buffer();
            struct addrinfo *res;
            int ret = getaddrinfo(packet->data, packet->data + packet->nodeLen, &packet->hints, &res);
            delete receiver;

            if (ret == 0) {
                int packetLength = res->ai_addrlen + sizeof(struct addrinfo);
                uint8_t* packet = new uint8_t[packetLength];
                memcpy(packet, res, sizeof(struct addrinfo));
                memcpy(packet + sizeof(struct addrinfo), res->ai_addr, res->ai_addrlen);
                if (Message::sendBuffer(msg.from, packet, packetLength) != M_OK) {
                    MONAPI_WARN("failed to send buffer %s:%d", __func__, __LINE__);
                }
                delete[] packet;
            } else {
                if (Message::sendBuffer(msg.from, NULL, 0) != M_OK) {
                    MONAPI_WARN("failed to send buffer %s:%d", __func__, __LINE__);
                }
            }
            freeaddrinfo(res);
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s:%d", __func__, __LINE__);
            }
            break;
        }
        case MSG_NET_SELECT:
        {
            int nfds = msg.arg1;
            bool haveReadFds = msg.str[0];
            bool haveWriteFds = msg.str[1];
            bool haveExceptFds = msg.str[2];
            bool haveTimeout = msg.str[3];
            fd_set* readfds = haveReadFds ? (fd_set*)&msg.str[4] : NULL;
            fd_set* writefds = haveWriteFds ? (fd_set*)&msg.str[4 + sizeof(fd_set)] : NULL;
            fd_set* exceptfds = haveExceptFds ? (fd_set*)&msg.str[4 + sizeof(fd_set) * 2] : NULL;
            struct timeval* timeout = haveTimeout ? (struct timeval*)&msg.str[4 + sizeof(fd_set) * 3] : NULL;

            int ret = select(nfds, readfds, writefds, exceptfds, timeout);
            if (haveReadFds) {
                *((fd_set*)msg.str) = *readfds;
            }
            if (haveWriteFds) {
                *((fd_set*)(&msg.str[sizeof(fd_set)])) = *writefds;
            }
            if (haveExceptFds) {
                *((fd_set*)(&msg.str[sizeof(fd_set) * 2])) = *exceptfds;
            }
            if (haveTimeout) {
                *((struct timeval*)(&msg.str[sizeof(fd_set) * 3])) = *timeout;
            }
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }
        case MSG_NET_SOCKET_CONTROL:
        {
            int sockfd = msg.arg1;
            long cmd = msg.arg2;
            bool haveArgp = msg.arg3;
            uint32_t* argp = NULL;

            if (haveArgp) {
                argp = (uint32_t*)msg.str;
            }

            int ret = ioctlsocket(sockfd, cmd, argp);
            if (Message::reply(&msg, ret, errno) != M_OK) {
                MONAPI_WARN("failed to reply %s", __func__);
            }
            break;
        }

        }
    }
}

void message_loop_init()
{
    sys_thread_new((char*)"message_loop", messageLoop, NULL, 0, 0);
}

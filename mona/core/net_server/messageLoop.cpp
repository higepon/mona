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
        case MSG_NET_SOCKET_SOCK:
        {
            int domain = msg.arg1;
            int type = msg.arg2;
            int protocol = msg.arg3;
            int ret = socket(domain, type, protocol);
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
        }
    }
}

void message_loop_init()
{
    sys_thread_new((char*)"message_loop", messageLoop, NULL, 0, 0);
}

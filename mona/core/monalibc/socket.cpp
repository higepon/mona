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
    uintptr_t nodeLen = strlen(node) + 1;
    uintptr_t serviceLen = strlen(service) + 1;
    uintptr_t structSize = sizeof(GetaddrinfoPacket) + nodeLen + serviceLen;
    uint8_t* p = new uint8_t[structSize];
    GetaddrinfoPacket* pack = (GetaddrinfoPacket*)p;
    pack->nodeLen = nodeLen;
    pack->hints = *hints;
    strcpy(pack->data, node);
    strcpy(pack->data + nodeLen, service);

    uintptr_t id = monapi_get_server_thread_id(ID_NET_SERVER);
    if (Message::send(id, MSG_NET_GET_ADDR_INFO) != M_OK) {
        return EBADF;
    }
    if (Message::sendBuffer(id, p, structSize) != M_OK) {
        return EBADF;
    }
    BufferReceiver* receiver = Message::receiveBuffer(id);
    ASSERT(receiver->bufferSize() == sizeof(addrinfo) || receiver->bufferSize() == 0);
    if (receiver->bufferSize() != 0) {
        *res = new struct addrinfo;
        **res = *((struct addrinfo*)receiver->buffer());
    }
    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_RESULT_OK;
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

}

int socket(int domain, int type, int protocol)
{
    uintptr_t id = monapi_get_server_thread_id(ID_NET_SERVER);
    MessageInfo ret;
    if (Message::sendReceive(&ret, id, MSG_NET_SOCKET_SOCK, domain, type, protocol) != M_OK) {
        return EINVAL;
    }
    return ret.arg2;
}

int send(int sockfd, void* buf, size_t len, int flags)
{
    uintptr_t id = monapi_get_server_thread_id(ID_NET_SERVER);
    if (Message::send(id, MSG_NET_SOCKET_SEND, sockfd, len, flags) != M_OK) {
        return EBADF;
    }

    if (Message::sendBuffer(id, buf, len) != M_OK) {
        return EBADF;
    }

    MessageInfo src;
    MessageInfo dst;
    src.from = id;
    src.header = MSG_RESULT_OK;
    src.arg1 = MSG_NET_SOCKET_SEND;
    if (Message::receive(&dst, &src, Message::equalsFromHeaderArg1) != M_OK) {
        return EBADF;
    }
    errno = dst.arg3;
    return dst.arg2;
}

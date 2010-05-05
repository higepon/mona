/*
 * http.cpp - 
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
 *  $Id: http.cpp 183 2008-07-04 06:19:28Z higepon $
 */

#include <monapi.h>
#include <monalibc.h>
#include "http.h"
#include <sys/socket.h>
#include <netdb.h>

static Buffer* get_by_addr(struct addrinfo* addr, const char* host, const char* path)
{
    int sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (-1 == sock) {
        return NULL;
    }

    if (connect(sock, addr->ai_addr, addr->ai_addrlen) != 0) {
        return NULL;
    }

    const size_t MAX_REQUEST_SIZE = 1024;
    char reqbuf[MAX_REQUEST_SIZE];
    snprintf(reqbuf, MAX_REQUEST_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);

    if (send(sock, reqbuf, strlen(reqbuf), 0) < 0) {
        closesocket(sock);
        return NULL;
    }

    const int RECV_BUFFER_SIZE = 8192;
    char recvBuffer[RECV_BUFFER_SIZE];

    Buffer* buffer = new Buffer(8192);
    int readSize = 0;
    while ((readSize = recv(sock, recvBuffer, RECV_BUFFER_SIZE, 0)) > 0) {
        buffer->append(recvBuffer, readSize);
    }
    closesocket(sock);
    const int bufferSize = buffer->size();
    const uint8_t* p = buffer->get();
    for (int i = 0; i < bufferSize; i++) {
        if (i + 4 < bufferSize &&
            p[i + 0] == '\r' &&
            p[i + 1] == '\n' &&
            p[i + 2] == '\r' &&
            p[i + 3] == '\n') {
            buffer->forwardPointer(i);
            return buffer;
        }
    }
    delete buffer;
    return NULL;
}

Buffer* http_get(const char* host, const char* path)
{
    int error;
    struct addrinfo hints;
    struct addrinfo* res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    if ((error = getaddrinfo(host, "80", &hints, &res)) != 0) {
        return NULL;
    }

    Buffer* buffer = NULL;;
    for (struct addrinfo* rp = res; rp != NULL; rp = rp->ai_next) {
        buffer = get_by_addr(rp, host, path);
        if (buffer != NULL) {
            break;
        }
    }
    freeaddrinfo(res);
    return buffer;
}


// int main(int argc, char* argv[])
// {
//     Buffer* buffer = http_get("www.monaos.org", "/");
//     _printf("buffer=%x", buffer);
//     for (int i = 0; i < buffer->size(); i++) {
//         _printf("%c", buffer->get()[i]);
//     }
//     _printf("END");
//     // delete buf
//     return 0;
// }

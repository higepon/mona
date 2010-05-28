#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <monapi.h>
#include <stdio.h>

using namespace MonAPI;

static void testSendInvalidFd()
{
    errno = 0;
    EXPECT_EQ(-1, send(-1, NULL, 0, 0));
    EXPECT_EQ(EBADF, errno);
}

static void testGetAddrInfo()
{
    struct addrinfo hints;
    struct addrinfo* res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("www.monaos.org", "80", &hints, &res));
    EXPECT_EQ(SOCK_STREAM, res->ai_socktype);
    EXPECT_EQ(AF_INET, res->ai_family);
    freeaddrinfo(res);
}

static void testSocket()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);
    }
    freeaddrinfo(res);
}

static void testConnect()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);
    }
    freeaddrinfo(res);
}

static void testReceive()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);

        const char* message = "status=Yay! Hello, World!. Tweet from Mona OS 0.4.0";
        const char* base64 = "";
        char reqbuf[1024];
        sprintf(reqbuf, "POST /1/statuses/update.xml HTTP/1.1\r\nHost: api.twitter.com\r\nAuthorization: Basic %s\r\nContent-Length: %d\r\n\r\n%s\r\n",
                base64, strlen(message), message);

        EXPECT_TRUE(send(sock, reqbuf, strlen(reqbuf), 0) > 0);
        char buf[127];
        int readSize = recv(sock, buf, 127, 0);
        do {
//             for (int i = 0; i < readSize; i++) {
//                 printf("%c", buf[i]);
//             }
        } while ((readSize = recv(sock, buf, 127, 0)) > 0);

        EXPECT_EQ(0, closesocket(sock));
    }
    freeaddrinfo(res);
}

static void testSocketOption()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);

        socklen_t n = 1;
        EXPECT_EQ(-1, setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n)));

        uint8_t buf;
        EXPECT_EQ(-1, getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &buf, &n));

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);

        int val = 1;
        EXPECT_EQ(0, ioctlsocket(sock, FIONBIO, &val));
        EXPECT_EQ(0, shutdown(sock, SHUT_RD));
    }
    freeaddrinfo(res);
}

#define PORT    8823
#define MAXDATA 1024
static void testEcho()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_TRUE(sock != -1);

    struct sockaddr_in addr;
    memset(&addr, sizeof(addr), 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(80);

    ASSERT_EQ(0, bind(sock, (struct sockaddr *)&addr, sizeof(addr)));
    ASSERT_EQ(0, listen(sock, 5));

    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100;

    int retval = select(sock + 1, &rfds, NULL, NULL, &tv);

    // timeout
    ASSERT_TRUE(retval == 0);

    closesocket(sock);

// needs access from clinet
#if 0
    struct sockaddr_in waddr;
    socklen_t writer_len;

    int s = accept(sock, (struct sockaddr*)&waddr, &writer_len);

    ASSERT_TRUE(s != -1);

    uint8_t buf[127];
    int readSize = recv(s, buf, 127, 0);
    do {
        for (int i = 0; i < readSize; i++) {
            printf("%c", buf[i]);
        }
    } while ((readSize = recv(sock, buf, 127, 0)) > 0);
    const char* res = "HTTP/1.1 200\r\n\r\n\r\nHello";
    send(s, (void*)res, strlen(res), 0);
#endif
}

static void testOpenManyTimes()
{
    for(int i = 0; i < 100; i++)
    {
        struct addrinfo hints;
        struct addrinfo* res;
        struct addrinfo* rp;
        
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        
        EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));
        
        for (rp = res; rp != NULL; rp = rp->ai_next) {
            int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            EXPECT_TRUE(sock != -1);
            
            int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
            EXPECT_EQ(0, ret);

            EXPECT_EQ(0, closesocket(sock));
        }
        freeaddrinfo(res);
    }
}

int main(int argc, char *argv[])
{
    testSendInvalidFd();
    testGetAddrInfo();
    testSocket();
    testConnect();
    testReceive();
    testSocketOption();
    testEcho();
    testOpenManyTimes();

    // todo close

    TEST_RESULTS(monapi_socket);
    return 0;
}

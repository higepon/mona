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
            for (int i = 0; i < readSize; i++) {
                printf("%c", buf[i]);
            }
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

        int n = 1;
        EXPECT_EQ(-1, setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n)));

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);
    }
    freeaddrinfo(res);
}


int main(int argc, char *argv[])
{
    testSendInvalidFd();
    testGetAddrInfo();
    testSocket();
    testConnect();
    testReceive();
    testSocketOption();

    // todo close

    TEST_RESULTS(monapi_socket);
    return 0;
}

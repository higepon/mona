#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

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

int main(int argc, char *argv[])
{
    testSendInvalidFd();
    testGetAddrInfo();
    testSocket();
    testConnect();

    TEST_RESULTS(monapi_socket);
    return 0;
}

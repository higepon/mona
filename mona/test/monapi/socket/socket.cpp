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
    freeaddrinfo(res);
}

int main(int argc, char *argv[])
{
    testSendInvalidFd();
    testGetAddrInfo();


    TEST_RESULTS(monapi_socket);
    return 0;
}

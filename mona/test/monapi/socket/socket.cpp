#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <sys/socket.h>
#include <errno.h>

using namespace MonAPI;

int main(int argc, char *argv[])
{
    errno = 0;
    EXPECT_EQ(-1, send(-1, NULL, 0, 0));
    EXPECT_EQ(EBADF, errno);
    TEST_RESULTS(monapi_socket);
    return 0;
}

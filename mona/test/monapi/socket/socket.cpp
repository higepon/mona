#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <sys/socket.h>

using namespace MonAPI;

int main(int argc, char *argv[])
{
    EXPECT_EQ(EINVAL, send(-1, NULL, 0, 0));

    TEST_RESULTS(monapi_socket);
    return 0;
}

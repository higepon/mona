#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

int main(int argc, char *argv[])
{
    TEST_RESULTS(monapi_socket);
    return 0;
}

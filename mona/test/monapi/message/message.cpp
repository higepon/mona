#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

void testSendBuffer()
{
}

int main(int argc, char *argv[])
{
    testSendBuffer();
    TEST_RESULTS(monapi_message);
    return 0;
}

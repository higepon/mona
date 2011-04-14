#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

int main(int argc, char *argv[])
{
    TEST_RESULTS(process_server);
    return 0;
}

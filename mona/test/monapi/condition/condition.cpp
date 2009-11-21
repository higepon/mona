#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

void testCondition()
{
    ASSERT_TRUE(true);
}

int main(int argc, char *argv[])
{
    testCondition();

    TEST_RESULTS(condition);
    return 0;
}

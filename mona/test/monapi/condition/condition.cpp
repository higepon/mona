#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;


void testCondition()
{
    intptr_t condition = syscall_condition_create();
    ASSERT_TRUE(condition > 0);

    ASSERT_EQ(M_OK, syscall_condition_destroy(condition));
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_destroy(3));
}

int main(int argc, char *argv[])
{
    testCondition();

    TEST_RESULTS(condition);
    return 0;
}

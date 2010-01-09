#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

void testDate()
{
    uint64_t msec1 = Date::nowInMsec();
    sleep(100);
    uint64_t msec2 = Date::nowInMsec();
    uint64_t diff = msec2 - msec1;
    EXPECT_TRUE(diff > 70);
    EXPECT_TRUE(diff < 130);
}

void testThreadSelf()
{
    uintptr_t self = syscall_mthread_self();
    EXPECT_EQ(self, System::getThreadID());
}

int main(int argc, char *argv[])
{
    testDate();
    testThreadSelf();

    TEST_RESULTS(monapi_misc);
    return 0;
}

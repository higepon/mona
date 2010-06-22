#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static void test_syscall_allocate_contiguous()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // address is out of range
    EXPECT_EQ(M_BAD_ADDRESS, syscall_allocate_contiguous(0x100, 4196));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    intptr_t size = syscall_allocate_contiguous(0x90000000, 8193);
    logprintf("size=%x", size);
    EXPECT_TRUE(size > 8193);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    uintptr_t* p = (uintptr_t*)0x90000000;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    *p = 0x12345678;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    EXPECT_EQ(0x12345678, *p);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    uintptr_t physAddress1 = syscall_get_physical_address(0x90000000);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    uintptr_t physAddress2 = syscall_get_physical_address(0x90001000);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    EXPECT_EQ(4096, physAddress2 - physAddress1);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

int main(int argc, char *argv[])
{
    test_syscall_allocate_contiguous();

    TEST_RESULTS(syscall);
    return 0;
}

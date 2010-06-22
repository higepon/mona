#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static void test_syscall_allocate_contiguous()
{
    // address is out of range
    EXPECT_EQ(M_BAD_ADDRESS, syscall_allocate_contiguous(0x100, 4196));
    EXPECT_EQ(M_OK, syscall_allocate_contiguous(0x91000000, 8193));
    uintptr_t* p = (uintptr_t*)0x91000000;
    *p = 0x12345678;
    EXPECT_EQ(0x12345678, *p);
    uintptr_t physAddress1 = syscall_get_physical_address(0x91000000);
    uintptr_t physAddress2 = syscall_get_physical_address(0x91001000);
    EXPECT_EQ(4096, physAddress2 - physAddress1);
}

int main(int argc, char *argv[])
{
    test_syscall_allocate_contiguous();

    TEST_RESULTS(syscall);
    return 0;
}

#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

enum {
    MSG_SEND_TEST
};

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

static void __fastcall infiniteThread(void* arg)
{
    for (;;) {
        MessageInfo msg;
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
    }
    exit(0);
}


void testThreadKill()
{
    uintptr_t tid = syscall_mthread_create_with_arg(infiniteThread, NULL);
    EXPECT_EQ(M_OK, Message::send(tid, MSG_SEND_TEST));
    EXPECT_EQ(M_OK, syscall_mthread_kill(tid));
    SKIP(EXPECT_EQ(M_BAD_THREAD_ID, Message::send(tid, MSG_SEND_TEST)));
}

void testNet()
{
    EXPECT_EQ(0x3412, htons(0x1234));
}

static void testISO9600_file_read()
{
    intptr_t handle = monapi_file_open("/LIBS/SCHEME/fib.scm", 0);
    monapi_cmemoryinfo* cmi = monapi_file_read(handle, 4096);
    ASSERT_TRUE(cmi != NULL);
    EXPECT_TRUE(cmi->Size > 0);
    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);

    // reached EOF
    monapi_cmemoryinfo* cmi2 = monapi_file_read(handle, 4096);
    EXPECT_TRUE(NULL == cmi2);
    monapi_file_close(handle);
}

static void test_Page_fault_handler_should_be_fast()
{
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
    const int SIZE = 3 * 1024 * 1024;
    ASSERT_EQ(M_OK, monapi_cmemoryinfo_create(mi, SIZE, MONAPI_FALSE));
    uint64_t s1 = MonAPI::Date::nowInMsec();
    memset(mi->Data, 0, SIZE);
    uint64_t e1 = MonAPI::Date::nowInMsec();
    EXPECT_TRUE((e1 - s1) <= 10);

    uint64_t s2 = MonAPI::Date::nowInMsec();
    memset(mi->Data, 0, SIZE);
    uint64_t e2 = MonAPI::Date::nowInMsec();
    EXPECT_EQ(0, e2 - s2);
    monapi_cmemoryinfo_delete(mi);
    monapi_cmemoryinfo_dispose(mi);
}

int main(int argc, char *argv[])
{
    testDate();
    testThreadSelf();
    testNet();
    testThreadKill();
    testISO9600_file_read();
    test_Page_fault_handler_should_be_fast();
    TEST_RESULTS(monapi_misc);
    return 0;
}

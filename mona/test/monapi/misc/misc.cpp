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
    EXPECT_EQ(M_BAD_THREAD_ID, Message::send(tid, MSG_SEND_TEST));
}

void testNet()
{
    EXPECT_EQ(0x3412, htons(0x1234));
}

int main(int argc, char *argv[])
{
    testDate();
    testThreadSelf();
    testNet();
    testThreadKill();
    TEST_RESULTS(monapi_misc);
    return 0;
}

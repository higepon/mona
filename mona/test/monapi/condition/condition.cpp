#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static intptr_t condition;
static intptr_t mutex;
static bool conditionOK;

enum {
    MSG_STARTUP       = 1234,
    forbidden_comma
};


static uintptr_t waitSubThread()
{
    MessageInfo dst, src;
    src.header = MSG_STARTUP;
    int ret = Message::receive(&dst, &src, Message::equalsHeader);
    ASSERT_EQ(M_OK, ret);
    uintptr_t tid = dst.from;
    return tid;
}


static void __fastcall conditionSubThread(void* mainThread)
{
    // waiting thread should get lock
    EXPECT_EQ(M_OK, syscall_mutex_lock(mutex));

    MessageInfo msg;
    intptr_t ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    while (!conditionOK) {
        EXPECT_EQ(M_OK, syscall_condition_wait(condition, mutex));
    }

    EXPECT_EQ(M_OK, syscall_mutex_unlock(mutex));

    Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
    }
}


void testCondition()
{
    condition = syscall_condition_create();
    ASSERT_TRUE(condition > 0);

    mutex = syscall_mutex_create();
    ASSERT_TRUE(mutex > 0);

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(conditionSubThread, (void*)mainThread);

    sleep(20);

    // wait sub thread starting up
    uintptr_t tid = waitSubThread();

    EXPECT_EQ(M_OK, syscall_mutex_lock(mutex));

    conditionOK = true;
    EXPECT_EQ(M_OK, syscall_condition_notify_all(condition));

    EXPECT_EQ(M_OK, syscall_mutex_unlock(mutex));

    waitSubThread();

    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_notify_all(3));

    ASSERT_EQ(M_OK, syscall_condition_destroy(condition));
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_destroy(3));
}

int main(int argc, char *argv[])
{
    testCondition();

    TEST_RESULTS(condition);
    return 0;
}

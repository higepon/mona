#include <monapi.h>
#include <monapi/Condition.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static Condition* cond;
static Mutex* mut;
static cond_t condition;
static mutex_t mutex;
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

// Test case (1)
//
// =====================================
//      main        |    sub thread
//   cond create    |
//   mutex create   |
//                  |    mutex lock
//   mutex lock     |    cond wait loop
//   cond = true    |
//   cond notify    |
//   mutex unlock   |
//                  |    cond loop exit
// =====================================
static void __fastcall conditionSubThread(void* mainThread)
{
    // waiting thread should get lock
    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    MessageInfo msg;
    intptr_t ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    while (!conditionOK) {
        EXPECT_EQ(M_OK, syscall_condition_wait(&condition, &mutex));
    }

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

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
    ASSERT_EQ(M_OK, syscall_condition_create(&condition));

    ASSERT_EQ(M_OK, syscall_mutex_create(&mutex));

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(conditionSubThread, (void*)mainThread);

    sleep(20);

    // wait sub thread starting up
    waitSubThread();

    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    conditionOK = true;
    EXPECT_EQ(M_OK, syscall_condition_notify_all(&condition));

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

    waitSubThread();

    cond_t invalidCond = 3;
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_notify_all(&invalidCond));

    ASSERT_EQ(M_OK, syscall_condition_destroy(&condition));
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_destroy(&invalidCond));
}

// Test case (2)
//
// =========================================================
//      main        |    sub thread     |    sub thread 2
//   cond create    |                   |
//   mutex create   |                   |
//                  |    mutex lock     |    mutex lock
//   mutex lock     |    cond wait loop |    cond wait loop
//   cond = true    |                   |
//   cond notify    |                   |
//   mutex unlock   |                   |
//                  |    cond loop exit |    cond loop exit
// =========================================================

void testCondition2()
{
    ASSERT_EQ(M_OK, syscall_condition_create(&condition));

    ASSERT_EQ(M_OK, syscall_mutex_create(&mutex));

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(conditionSubThread, (void*)mainThread);
    syscall_mthread_create_with_arg(conditionSubThread, (void*)mainThread);


    // wait sub thread starting up
    waitSubThread();

    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    conditionOK = true;
    EXPECT_EQ(M_OK, syscall_condition_notify_all(&condition));

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

    waitSubThread();
    waitSubThread();

    ASSERT_EQ(M_OK, syscall_condition_destroy(&condition));
    cond_t invalidCond = 3;
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_destroy(&invalidCond));
}

// Test case (3)
//
// =====================================
//      main        |    sub thread
//   cond create    |
//   mutex create   |
//                  |    mutex lock
//   mutex lock     |    cond wait timeout loop
//   cond = true    |
//   cond notify    |
//   mutex unlock   |
//                  |    cond loop exit
// =====================================
static void __fastcall conditionSubThread3(void* mainThread)
{
    // waiting thread should get lock
    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    MessageInfo msg;
    intptr_t ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    while (!conditionOK) {
        EXPECT_EQ(M_OK, syscall_condition_wait_timeout(&condition, &mutex, 1000));
    }

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

    Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
    }
}

void testCondition3()
{
    ASSERT_EQ(M_OK, syscall_condition_create(&condition));

    ASSERT_EQ(M_OK, syscall_mutex_create(&mutex));

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(conditionSubThread3, (void*)mainThread);

    sleep(20);

    // wait sub thread starting up
    waitSubThread();

    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    conditionOK = true;
    EXPECT_EQ(M_OK, syscall_condition_notify_all(&condition));

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

    waitSubThread();

    cond_t invalidCond = 3;
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_notify_all(&invalidCond));

    ASSERT_EQ(M_OK, syscall_condition_destroy(&condition));
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_destroy(&invalidCond));
}

// Test case (4)
//
// =====================================
//      main        |    sub thread
//   cond create    |
//   mutex create   |
//                  |    mutex lock
//                  |    cond wait timeout loop
//                  |    timeout
//   mutex lock     |    cond wait loop
//   notify         |
//   mutex unlock   |
//                  |    cond loop exit
// =====================================
static void __fastcall conditionSubThread4(void* mainThread)
{
    // waiting thread should get lock
    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    EXPECT_EQ(M_TIMED_OUT, syscall_condition_wait_timeout(&condition, &mutex, 10));

    MessageInfo msg;
    intptr_t ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    while (!conditionOK) {
        EXPECT_EQ(M_OK, syscall_condition_wait_timeout(&condition, &mutex, 1000));
    }

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

    Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
    }
}

void testCondition4()
{
    ASSERT_EQ(M_OK, syscall_condition_create(&condition));

    ASSERT_EQ(M_OK, syscall_mutex_create(&mutex));

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(conditionSubThread4, (void*)mainThread);

    // wait sub thread starting up
    waitSubThread();

    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutex));

    conditionOK = true;
    EXPECT_EQ(M_OK, syscall_condition_notify_all(&condition));

    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutex));

    waitSubThread();

    cond_t invalidCond = 3;
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_notify_all(&invalidCond));

    ASSERT_EQ(M_OK, syscall_condition_destroy(&condition));
    EXPECT_EQ(M_BAD_CONDITION_ID, syscall_condition_destroy(&invalidCond));
}

// Test case (5)
//
// =====================================
//      main        |    sub thread
//   cond create    |
//   mutex create   |
//                  |    mutex lock
//   mutex lock     |    cond wait loop
//   cond = true    |
//   cond notify    |
//   mutex unlock   |
//                  |    cond loop exit
// =====================================
static void __fastcall conditionSubThread5(void* mainThread)
{
    // waiting thread should get lock
    EXPECT_EQ(M_OK, mut->lock());

    MessageInfo msg;
    intptr_t ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    while (!conditionOK) {
        EXPECT_EQ(M_OK, cond->wait(mut));
    }

    EXPECT_EQ(M_OK, mut->unlock());

    Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
    }
}

void testCondition5()
{
    cond = new Condition();
    ASSERT_TRUE(condition != NULL);

    mut = new Mutex();
    ASSERT_TRUE(mut != NULL);

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(conditionSubThread5, (void*)mainThread);

    sleep(20);

    // wait sub thread starting up
    waitSubThread();

    EXPECT_EQ(M_OK, mut->lock());

    conditionOK = true;
    EXPECT_EQ(M_OK, cond->notifyAll());

    EXPECT_EQ(M_OK, mut->unlock());

    waitSubThread();

    delete cond;
    delete mut;
}

int main(int argc, char *argv[])
{
    testCondition();
    testCondition2();
    testCondition3();
    testCondition4();
    testCondition5();

    TEST_RESULTS(condition);
    return 0;
}

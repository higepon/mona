#include <monapi.h>
#include <monapi/Mutex.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static Mutex* mutex;
static intptr_t mutexid;
enum {
    MSG_STARTUP       = 1234,
    MSG_PLEASE_UNLOCK,
    MSG_UNLOCKED
};

uintptr_t waitSubThread()
{
    MessageInfo dst, src;
    src.header = MSG_STARTUP;
    int ret = Message::receive(&dst, &src, Message::equalsHeader);
    ASSERT_EQ(M_OK, ret);
    uintptr_t tid = dst.from;
    return tid;
}

void __fastcall mutexSubThread(void* mainThread)
{
    ASSERT_EQ(M_OK, syscall_mutex_lock_timeout(mutexid, 10));
    MessageInfo msg;
    int ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        if (msg.header == MSG_PLEASE_UNLOCK) {
            ASSERT_EQ(M_OK, syscall_mutex_unlock(mutexid));

            int ret = Message::reply(&msg);
            ASSERT_EQ(M_OK, ret);
        }
    }
}

void __fastcall mutexClassSubThread(void* mainThread)
{
    ASSERT_EQ(M_OK, mutex->lock(10));
    MessageInfo msg;
    int ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        if (msg.header == MSG_PLEASE_UNLOCK) {
            ASSERT_EQ(M_OK, mutex->unlock());

            int ret = Message::reply(&msg);
            ASSERT_EQ(M_OK, ret);
        }
    }
}

void testSyscallMutex()
{
    mutexid = syscall_mutex_create();
    ASSERT_GT(mutexid, 0);

    // Lock
    EXPECT_EQ(M_OK, syscall_mutex_lock(mutexid));

    // Lock twice is ok
    EXPECT_EQ(M_OK, syscall_mutex_lock(mutexid));

    // Bad mutexid
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_lock(3000));

    // fetch
    EXPECT_EQ(mutexid, syscall_mutex_fetch(mutexid));

    // fetch invalid
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_fetch(1234));

    // Unlock
    EXPECT_EQ(M_OK, syscall_mutex_unlock(mutexid));

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(mutexSubThread, (void*)mainThread);

    // wait sub thread starting up
    uintptr_t tid = waitSubThread();

    // Sub thread has lock, so timed out.
    EXPECT_EQ(M_TIMED_OUT, syscall_mutex_lock_timeout(mutexid, 10));

    // try lock fails
    EXPECT_EQ(M_BUSY, syscall_mutex_try_lock(mutexid));

    // try lock invalid mutexid
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_try_lock(1234));

    // make sub thread unlock the mutex
    MessageInfo msg;
    int ret = Message::sendReceive(&msg, tid, MSG_PLEASE_UNLOCK);
    ASSERT_EQ(M_OK, ret);

    // Lock ok
    EXPECT_EQ(M_OK, syscall_mutex_try_lock(mutexid));

    // unlock
    EXPECT_EQ(M_OK, syscall_mutex_unlock(mutexid));

    // destroy
    EXPECT_EQ(M_OK, syscall_mutex_destroy(mutexid));

    // destroy invalid mutex
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_destroy(1234));
}

void testClassMutex()
{
    mutex = new Mutex();
    ASSERT_TRUE(mutex != NULL);

    // Lock
    EXPECT_EQ(M_OK, mutex->lock());

    // Lock twice is ok
    EXPECT_EQ(M_OK, mutex->lock());

    // fetch
    Mutex* fetched = new Mutex(mutex->getId());
    EXPECT_EQ(mutex->getId(), fetched->getId());

    // fetch invalid
    Mutex* invalidFetched = new Mutex(1234);
    EXPECT_EQ(M_BAD_MUTEX_ID, invalidFetched->getId());

    // Unlock
    EXPECT_EQ(M_OK, mutex->unlock());

    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(mutexClassSubThread, (void*)mainThread);

    // wait sub thread starting up
    uintptr_t tid = waitSubThread();

    // Sub thread has lock, so timed out.
    EXPECT_EQ(M_TIMED_OUT, mutex->lock(10));

    // try lock fails
    EXPECT_EQ(M_BUSY, mutex->tryLock());

    // make sub thread unlock the mutex
    MessageInfo msg;
    int ret = Message::sendReceive(&msg, tid, MSG_PLEASE_UNLOCK);
    ASSERT_EQ(M_OK, ret);

    // Lock ok
    EXPECT_EQ(M_OK, mutex->tryLock());

    // unlock
    EXPECT_EQ(M_OK, mutex->unlock());

    // destroy
    EXPECT_EQ(M_OK, mutex->destroy());

    // destroy
    delete mutex;
}

void __fastcall consumer(void* mainThread)
{
    mutex->lock();
    MessageInfo msg;
    if (Message::send((uintptr_t)mainThread, MSG_STARTUP) != 0) {
        printf("Mutex test fatal\n");
        exit(-1);
    }

    // wait forever
    for (;;) {
        if (Message::receive(&msg) != 0) {
            continue;
        }
    }
}

int main(int argc, char *argv[])
{
    testSyscallMutex();
    testClassMutex();

    TEST_RESULTS(syscal_mutex);
    return 0;
}

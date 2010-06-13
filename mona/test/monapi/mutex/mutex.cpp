#include <monapi.h>
#include <monapi/Mutex.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static Mutex* mutex;
static mutex_t mutexid;
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
    ASSERT_EQ(M_OK, syscall_mutex_lock_timeout(&mutexid, 10));
    MessageInfo msg;
    int ret = Message::send((uintptr_t)mainThread, MSG_STARTUP, System::getThreadID());
    ASSERT_EQ(M_OK, ret);

    // wait forever
    for (;;) {
        if (Message::receive(&msg) != M_OK) {
            continue;
        }
        if (msg.header == MSG_PLEASE_UNLOCK) {
            ASSERT_EQ(M_OK, syscall_mutex_unlock(&mutexid));

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
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    int beforeCount = syscall_mutex_count();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    ASSERT_EQ(M_OK, syscall_mutex_create(&mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    ASSERT_EQ(beforeCount + 1, syscall_mutex_count());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // Lock
    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // Lock twice is ok
    EXPECT_EQ(M_OK, syscall_mutex_lock(&mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    mutex_t invalidMutex = 3000;
    // Bad mutexid
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_lock(&invalidMutex));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // fetch
    mutex_t dest;
    EXPECT_EQ(M_OK, syscall_mutex_fetch(&dest, &mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // fetch invalid
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_fetch(&dest, &invalidMutex));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // Unlock
    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    uintptr_t mainThread = System::getThreadID();
    syscall_mthread_create_with_arg(mutexSubThread, (void*)mainThread);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // wait sub thread starting up
    uintptr_t tid = waitSubThread();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // Sub thread has lock, so timed out.
    EXPECT_EQ(M_TIMED_OUT, syscall_mutex_lock_timeout(&mutexid, 10));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // try lock fails
    EXPECT_EQ(M_BUSY, syscall_mutex_try_lock(&mutexid));

    // try lock invalid mutexid
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_try_lock(&invalidMutex));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // make sub thread unlock the mutex
    MessageInfo msg;
    int ret = Message::sendReceive(&msg, tid, MSG_PLEASE_UNLOCK);
    ASSERT_EQ(M_OK, ret);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // Lock ok
    EXPECT_EQ(M_OK, syscall_mutex_try_lock(&mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // unlock
    EXPECT_EQ(M_OK, syscall_mutex_unlock(&mutexid));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // destroy
    //  Check reference counting.
    EXPECT_EQ(M_RELEASED, syscall_mutex_destroy(&mutexid));
    EXPECT_EQ(M_OK, syscall_mutex_destroy(&dest));
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // should be removed from IDManager
    ASSERT_EQ(beforeCount, syscall_mutex_count());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // destroy invalid mutex
    EXPECT_EQ(M_BAD_MUTEX_ID, syscall_mutex_destroy(&invalidMutex));
}

void testClassMutex()
{
    int beforeCount = syscall_mutex_count();

    mutex = new Mutex();
    ASSERT_TRUE(mutex != NULL);
    ASSERT_EQ(beforeCount + 1, syscall_mutex_count());

//     uintptr_t* p = (uintptr_t*)0;
//     *p = 1;


    // Lock
    EXPECT_EQ(M_OK, mutex->lock());

    // Lock twice is ok
    EXPECT_EQ(M_OK, mutex->lock());

    // fetch
    mutex_t m = mutex->getMutex_t();
    Mutex* fetched = new Mutex(&m);

    ASSERT_EQ(beforeCount + 2, syscall_mutex_count());

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
    EXPECT_EQ(M_RELEASED, mutex->destroy());
    EXPECT_EQ(M_OK, fetched->destroy());

    ASSERT_EQ(beforeCount, syscall_mutex_count());

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
//    DebuggerService::breakpoint();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    testSyscallMutex();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    testClassMutex();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    TEST_RESULTS(syscal_mutex);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return 0;
}

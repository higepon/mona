#include <monapi.h>

using namespace MonAPI;

static Mutex* mutex;

#define MSG_CONSUMER_STARTUP 1234

#define ASSERT_TRUE(condition) {if (!(condition)) {_printf("ASSERT_TRUE failure %s:%d: %s\n", __FILE__, __LINE__, #condition); exit(-1);}}
#define ASSERT_EQ(expected, actual) { {intptr_t v = (intptr_t)(actual); _printf("<%d %d>", v, expected);if ((expected) != v) {_printf("ASSERT_EQ failure %s:%d: %s expected but got %d\n", __FILE__, __LINE__, #expected, v); exit(-1);}}}

static intptr_t mutexid;

void waitConsumerStartup(uintptr_t tid)
{
    for (MessageInfo msg;;) {
        if (Message::receive(&msg) != 0) {
            continue;
        }

        if (msg.header == MSG_CONSUMER_STARTUP) {
            break;
        }
    }
}


void __fastcall mutexSubThread(void* mainThread)
{
    ASSERT_EQ(M_OK, syscall_mutex_lock_timeout(mutexid, 10));
    MessageInfo msg;
    if (Message::send((uintptr_t)mainThread, MSG_CONSUMER_STARTUP) != 0) {
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

void testSyscallMutexCreate()
{
    mutexid = syscall_mutex_create();
    ASSERT_TRUE(mutexid > 0);

    // Lock
    ASSERT_EQ(M_OK, syscall_mutex_lock(mutexid));

    // Lock twice is ok
    ASSERT_EQ(M_OK, syscall_mutex_lock(mutexid));

    // Bad mutexid
    ASSERT_EQ(M_BAD_MUTEX_ID, syscall_mutex_lock(3000));

    // Unlock
    ASSERT_EQ(M_OK, syscall_mutex_unlock(mutexid));


    uintptr_t mainThread = System::getThreadID();
    uintptr_t tid = syscall_mthread_create_with_arg(mutexSubThread, (void*)mainThread);
    waitConsumerStartup(tid);

    // Sub thread has lock, timed out.
    ASSERT_EQ(M_TIMED_OUT, syscall_mutex_lock_timeout(mutexid, 10));

//     // Not Timeout
//     ASSERT_EQ(M_OK, syscall_mutex_lock_timeout(mutexid, 10));

//     // Timeout
//     ASSERT_EQ(M_TIMED_OUT, syscall_mutex_lock_timeout(mutexid, 10));

    // to be tested
    // other thread gets lock, main timedout
    // other thread gets lock, main waits
    // check return value of each mutex functions

}



void __fastcall consumer(void* mainThread)
{
    mutex->lock();
    MessageInfo msg;
    if (Message::send((uintptr_t)mainThread, MSG_CONSUMER_STARTUP) != 0) {
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
    testSyscallMutexCreate();

    uintptr_t mainThread = System::getThreadID();
    mutex = new Mutex();
    uintptr_t tid = syscall_mthread_create_with_arg(consumer, (void*)mainThread);

    waitConsumerStartup(tid);

    if (Mutex::TIMEOUT == mutex->lock(50)) {
        printf("mutex test passed\n");
    } else {
        printf("mutex test failed\n");
    }

    syscall_mthread_kill(tid);
    return 0;
}

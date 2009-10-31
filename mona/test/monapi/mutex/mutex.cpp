#include <monapi.h>

using namespace MonAPI;

static Mutex* mutex;

#define MSG_CONSUMER_STARTUP 1234

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

int main(int argc, char *argv[])
{
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

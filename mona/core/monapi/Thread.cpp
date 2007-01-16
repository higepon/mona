/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/

#include <monapi.h>

using namespace MonAPI;

static uint32_t parent_tid = 0;
static void __fastcall runThread(void* arg)
{
    MonAPI::Message::send(parent_tid, MSG_SERVER_START_OK);
    for (MessageInfo info;;) {
        if (MonAPI::Message::receive(&info) != 0) continue;
        if (info.header == MSG_SERVER_START_OK) {
            void (*run)(void*) = (void(*)(void*))info.arg1;
            void (*notify)(void*) = (void(*)(void*))info.arg2;
            run(arg);
            if (notify)
            {
                notify(arg);
            }
            break;
        }
    }
    exit(0);
}

Thread::Thread(void (*run)(void*), void* arg, void (*notify)(void*)) : run_(run), notify_(notify), arg_(arg)
{
    parent_tid = MonAPI::System::getThreadID();
}

Thread::~Thread()
{
}

void Thread::start()
{
    threadId_ = mthread_create_with_arg(runThread, arg_);
    // wait for MSG_SERVER_START_OK message
    MessageInfo msg, src;
    src.header = MSG_SERVER_START_OK;
    MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
    uint32_t child = msg.from;
    MonAPI::Message::send(child, MSG_SERVER_START_OK, (uint32_t)run_, (uint32_t)notify_, (uint32_t)arg_);
}

void Thread::stop()
{
    mthread_kill(threadId_);
}

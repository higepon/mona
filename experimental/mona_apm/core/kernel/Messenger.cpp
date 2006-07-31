#include "Messenger.h"
#include "global.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
Messenger::Messenger(int size) : size_(size), allocated_(0) {

    info_ = new MessageInfo[size];
}

Messenger::~Messenger() {
}

MessageInfo* Messenger::allocateMessageInfo() {

    MessageInfo* result = &(info_[allocated_]);
    allocated_++;
    if (allocated_ > size_ - 1) {

#if 0  // DEBUG for message
        g_console->printf("***** msg buf index set to zero again ****");
#endif
        allocated_ = 0;
    }
    return result;
}

int Messenger::send(dword id, MessageInfo* message)
{
    Thread* thread;
    MessageInfo* info;

    if (message == (MessageInfo*)NULL)
    {
        return -1;
    }

    if ((thread = g_scheduler->Find(id)) == (Thread*)NULL)
    {
        return -1;
    }

    info = allocateMessageInfo();

#if 0
    logprintf("send:to=%x head=%x a1=%x a2=%x a3=%x from=%x\n"
              , id
              , message->header
              , message->arg1
              , message->arg2
              , message->arg3
              , message->from
        );
#endif

    *info = *message;
    info->from = g_currentThread->thread->id;

    thread->flags |= MEvent::MESSAGE;
    thread->messageList->add(info);

    g_scheduler->EventComes(thread, MEvent::MESSAGE);

    return 0;
}

int Messenger::receive(Thread* thread, MessageInfo* message)
{
    MessageInfo* from = thread->messageList->removeAt(0);

    if (from == (MessageInfo*)NULL)
    {
        return -1;
    }

#if 0
    logprintf("recv:to=%x head=%x a1=%x a2=%x a3=%x from=%x\n"
              , thread->id
              , message->header
              , message->arg1
              , message->arg2
              , message->arg3
              , message->from
        );
#endif

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;

    return 0;
}

int Messenger::peek(Thread* thread, MessageInfo* message, int index, int flags)
{
    List<MessageInfo*>* list = thread->messageList;

    if (index > list->size())
    {
        return 1;
    }

    MessageInfo* from = flags & PEEK_REMOVE ? list->removeAt(index) : list->get(index);

    if (from == (MessageInfo*)NULL)
    {
        return -1;
    }

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    return 0;
}

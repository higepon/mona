#include "Messenger.h"
#include "global.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
Messenger::Messenger()
{
}

Messenger::~Messenger()
{
}


intptr_t Messenger::send(uint32_t id, MessageInfo* message)
{
    Thread* thread;
    ASSERT(message != NULL);

    // todo this shoulbe outside
    if ((thread = g_scheduler->Find(id)) == (Thread*)NULL) {
        return -1;
    }

    if (thread->messageList->size() == MAX_MESSAGES) {
        return M_MESSAGE_OVERFLOW;
    }

    MessageInfo* info = new MessageInfo;

    *info = *message;
    info->from = g_currentThread->thread->id;

    thread->flags |= MEvent::MESSAGE;
    thread->messageList->add(info);

    g_scheduler->EventComes(thread, MEvent::MESSAGE);

    return M_OK;
}

int Messenger::receive(Thread* thread, MessageInfo* message)
{
    MessageInfo* from = thread->messageList->removeAt(0);
    if (from == NULL) {
        return -1;
    }

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    delete from;
    return M_OK;
}

int Messenger::peek(Thread* thread, MessageInfo* message, int index, int flags)
{
    List<MessageInfo*>* list = thread->messageList;

    if (index > list->size()) {
        return 1;
    }

    MessageInfo* from = flags & PEEK_REMOVE ? list->removeAt(index) : list->get(index);

    if (from == (MessageInfo*)NULL)
    {
        return -1;
    }

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    if (flags & PEEK_REMOVE) {
        delete from;
    }
    return M_OK;
}

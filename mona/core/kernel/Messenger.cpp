#include "Scheduler.h"
#include "Messenger.h"
#include "global.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
Messenger::Messenger(Scheduler* scheduler) : scheduler_(scheduler)
{
}

Messenger::~Messenger()
{
}


intptr_t Messenger::send(Thread* thread, MessageInfo* message)
{
    ASSERT(message != NULL);

    if (thread->messageList->size() == MAX_MESSAGES) {
        return M_MESSAGE_OVERFLOW;
    }

    MessageInfo* info = new MessageInfo;
    if (NULL == info) {
        logprintf("from=%s to=%s", g_currentThread->thread->tinfo->process->getName(), thread->tinfo->process->getName());
    }
    ASSERT(info != NULL);

    *info = *message;
    info->from = g_currentThread->thread->id;

    thread->flags |= MEvent::MESSAGE;
    thread->messageList->add(info);
    scheduler_->EventComes(thread, MEvent::MESSAGE);
    return M_OK;
}

intptr_t Messenger::receive(Thread* thread, MessageInfo* message)
{
    MessageInfo* from = thread->messageList->removeAt(0);
    if (from == NULL) {
        return M_MESSAGE_NOT_FOUND;
    }

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    delete from;
    return M_OK;
}

intptr_t Messenger::peek(Thread* thread, MessageInfo* message, int index, int flags)
{
    List<MessageInfo*>* list = thread->messageList;

    if (index >= list->size()) {
        return M_BAD_INDEX;
    }

    ASSERT(index >= 0);
    MessageInfo* from = flags & PEEK_REMOVE ? list->removeAt(index) : list->get(index);
    ASSERT(from != NULL);

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    if (flags & PEEK_REMOVE) {
        delete from;
    }
    return M_OK;
}

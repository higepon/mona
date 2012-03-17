
#include "global.h"
#include "Scheduler.h"
#include "Messenger.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
Messenger::Messenger(Scheduler* scheduler) : scheduler_(scheduler)
{
}

Messenger::~Messenger()
{
}

// don't logprint inside this function. See LogConsole::print
intptr_t Messenger::send(Thread* thread, MessageInfo* message)
{
    // ignore the idle thread.
    if (thread == g_idleThread) {
        return M_OK;
    }
    MONA_ASSERT(message != NULL);

    // Merge MSG_TIMER into one message
    if (message->header == MSG_TIMER) {
        if (thread->timerMessage == NULL) {
          MessageInfo* info = new MessageInfo;
          MONA_ASSERT(info != NULL);
          *info = *message;
          info->from = g_currentThread->thread->id;
          thread->timerMessage = info;
        } else {

        }
    } else {
      if (thread->messageList.size() == MAX_MESSAGES) {
#if 0
           for (int i = 0; i < thread->messageList->size(); i++) {
               logprintf("h=%x %s1=%x\n", thread->messageList->get(i)->header, thread->tinfo->process->getName(), thread->messageList->get(i)->arg1);
           }
#endif
           return M_MESSAGE_OVERFLOW;
      }

      MessageInfo* info = new MessageInfo;
      MONA_ASSERT(info != NULL);
      *info = *message;
      info->from = g_currentThread->thread->id;
      thread->messageList.add(info);
    }
    thread->flags |= MEvent::MESSAGE;
    scheduler_->EventComes(thread, MEvent::MESSAGE);
    return M_OK;
}

intptr_t Messenger::receive(Thread* thread, MessageInfo* message)
{
    MessageInfo* from = NULL;
    if (!thread->messageList.removeAt(0, &from)) {
      if (thread->timerMessage) {
        thread->flags &= ~MEvent::MESSAGE;
        *message = *(thread->timerMessage);
        delete thread->timerMessage;
        thread->timerMessage = NULL;
        return M_OK;
      } else {
        return M_MESSAGE_NOT_FOUND;
      }
    }
    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    delete from;
    return M_OK;
}

intptr_t Messenger::peek(Thread* thread, MessageInfo* message, int index, int flags)
{
    HList<MessageInfo*>& list = thread->messageList;

    if (list.size() == 0 && index == 0 && thread->timerMessage) {
      MessageInfo* from = thread->timerMessage;
      if (flags & PEEK_REMOVE) {
        delete thread->timerMessage;
        thread->timerMessage = NULL;
      }
      thread->flags &= ~MEvent::MESSAGE;
      *message = *from;
      return M_OK;
    } else if (list.size() == index && thread->timerMessage) {
      MessageInfo* from = thread->timerMessage;
      if (flags & PEEK_REMOVE) {
        delete thread->timerMessage;
        thread->timerMessage = NULL;
      }
      thread->flags &= ~MEvent::MESSAGE;
      *message = *from;
      return M_OK;
    }

    if (index >= list.size()) {
        return M_BAD_INDEX;
    }

    MONA_ASSERT(index >= 0);
    MessageInfo* from = NULL;
    if (flags & PEEK_REMOVE) {
        list.removeAt(index, &from);
    } else {
        from = list.get(index);
    }

    thread->flags &= ~MEvent::MESSAGE;
    *message = *from;
    if (flags & PEEK_REMOVE) {
        delete from;
    }
    return M_OK;
}

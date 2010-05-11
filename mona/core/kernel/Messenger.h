#ifndef _MONA_MESSENGER_
#define _MONA_MESSENGER_

#include <sys/types.h>
#include "Thread.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
class Messenger
{
public:
    Messenger(Scheduler* scheduler);
    virtual ~Messenger();

    intptr_t send(Thread* thread, MessageInfo* message);
    int receive(Thread* thread, MessageInfo* message);
    int peek(Thread* thread, MessageInfo* message, int index, int flags);

private:
    Scheduler* scheduler_;
};

#endif

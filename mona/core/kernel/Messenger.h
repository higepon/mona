#ifndef _MONA_MESSENGER_
#define _MONA_MESSENGER_

#include <sys/types.h>
#include "Thread.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
class Messenger {

  public:
    Messenger();
    virtual ~Messenger();

  public:
    intptr_t send(uint32_t tid, MessageInfo* message);
    int receive(Thread* thread, MessageInfo* message);
    int peek(Thread* thread, MessageInfo* message, int index, int flags);
};

#endif

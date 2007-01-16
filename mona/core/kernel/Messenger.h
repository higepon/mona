#ifndef _MONA_MESSENGER_
#define _MONA_MESSENGER_

#include <sys/types.h>
#include "Thread.h"

/*----------------------------------------------------------------------
    Messenger
----------------------------------------------------------------------*/
class Messenger {

  public:
    Messenger(int size);
    virtual ~Messenger();

  public:
    int send(const char* name, MessageInfo* message);
    int send(uint32_t tid, MessageInfo* message);
    int receive(Thread* thread, MessageInfo* message);
    int peek(Thread* thread, MessageInfo* message, int index, int flags);

  private:
    MessageInfo* allocateMessageInfo();

  private:
    int size_;
    int allocated_;
    MessageInfo* info_;
};

#endif

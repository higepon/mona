#ifndef _MONA_USERLIB_MESSAGE_
#define _MONA_USERLIB_MESSAGE_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
class Message {

  private:
    static HList<MessageInfo*>* queue;

  public:
    static void initialize();
    static int send(dword tid, MessageInfo* info);
    static int send(dword tid, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL);
    static int receive(MessageInfo* info);
    static int receive(MessageInfo* info, dword tid);
    static int receive(MessageInfo* info, dword tid, dword header);
    static int receive(MessageInfo* info, dword tid, dword header, dword arg1);
    static int sendReceive(MessageInfo* result, dword tid, MessageInfo* info);
    static int sendReceive(MessageInfo* result, dword tid, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL);
    static void create(MessageInfo* info, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL);
    static bool exist();
    static dword lookup(const char* name);
    static dword lookupMainThread(const char* name);
};

}

#endif

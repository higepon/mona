#ifndef _MONA_USERLIB_MESSAGE_
#define _MONA_USERLIB_MESSAGE_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
class Message {

public:
    static int send(dword tid, MessageInfo* info);
    static int send(dword tid, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL);
    static int receive(MessageInfo* info);
    static void create(MessageInfo* info, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL);
    static bool exist();
    static dword lookup(const char* name);
    static dword lookupMainThread(const char* name);
};

}

#endif

#ifndef _MONA_USERLIB_MESSAGE_
#define _MONA_USERLIB_MESSAGE_

#include <types.h>

/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
class Message {

  private:
    Message();
    virtual ~Message();

  public:
    static int send(dword pid, MessageInfo* info);
    static int receive(MessageInfo* info);
    static void create(MessageInfo* info, dword header, dword arg1, dword arg2, dword arg3, char* str);
    static bool exist();
    static dword lookup(const char* name);
};
#endif

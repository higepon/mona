/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

int Message::send(dword id, MessageInfo* info) {
    return syscall_send(id, info);
}

int Message::receive(MessageInfo* info) {

    int result = syscall_receive(info);
    if (result != 0) {
         syscall_mthread_yeild_message();
         result = syscall_receive(info);
    }
    return result;
}

dword Message::lookup(const char* name) {
    return syscall_lookup(name);
}

dword Message::lookupMainThread(const char* name) {
    return syscall_lookup_main_thread(name);
}

void Message::create(MessageInfo* info, dword header, dword arg1, dword arg2, dword arg3, char* str) {

    info->header = header;
    info->arg1 = arg1;
    info->arg2 = arg2;
    info->arg3 = arg3;

    if (str) {
        strncpy(info->str, str, sizeof(info->str));
    }
    return;
}

bool Message::exist()
{
    return (syscall_exist_message() == 1);
}

}

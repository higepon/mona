#include <monapi/messages.h>
#include <monapi/cmessage.h>

static dword get_file_server_tid()
{
    static dword ret = THREAD_UNKNOWN;
    if (ret == THREAD_UNKNOWN) ret = monapi_cmessage_lookup_main_thread("FILE.SVR");
    return ret;
}

monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, int prompt)
{
    MessageInfo msg;
    monapi_cmessage_send_receive_args(&msg, get_file_server_tid(), MSG_FILE_READ_DATA, prompt, 0, 0, file);
    if (msg.arg2 == 0) return NULL;

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

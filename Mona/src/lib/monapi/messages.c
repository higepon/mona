#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/cmessage.h>

static dword server_ids[] =
{
    THREAD_UNKNOWN,  // ID_MOUSE_SERVER
    THREAD_UNKNOWN,  // ID_KEYBOARD_SERVER
    THREAD_UNKNOWN,  // ID_FILE_SERVER
    THREAD_UNKNOWN,  // ID_GUI_SERVER
    THREAD_UNKNOWN,  // ID_ELF_SERVER
    THREAD_UNKNOWN   // ID_PROCESS_SERVER
};

static const char* server_names[] =
{
    "MOUSE.SVR", "KEYBDMNG.SVR", "FILE.SVR", "GUI.SVR", "ELF.SVR", "ELF.SVR"/*"PROCESS.SVR"*/
};

dword monapi_get_server_thread_id(int id)
{
    if (id < 0 || ID_NUMBER_OF_SERVERS <= id) return THREAD_UNKNOWN;

    if (server_ids[id] == THREAD_UNKNOWN)
    {
        server_ids[id] = monapi_cmessage_lookup_main_thread(server_names[id]);
        if (server_ids[id] == THREAD_UNKNOWN)
        {
            printf("ERROR: can not connect to %s\n", server_names[id]);
        }
    }
    return server_ids[id];
}

int monapi_call_dispose_handle(int id, dword handle)
{
    dword tid = monapi_get_server_thread_id(ID_MOUSE_SERVER);
    if (monapi_cmessage_send_args(tid, MSG_DISPOSE_HANDLE, handle, 0, 0, NULL) != 0)
    {
        return 0;
    }
    return 1;
}

int monapi_register_to_server(int id, int enabled)
{
    dword tid = monapi_get_server_thread_id(id), header = MSG_NONE;
    switch (id)
    {
        case ID_KEYBOARD_SERVER:
            header = enabled ? MSG_KEY_REGIST_TO_SERVER : MSG_KEY_UNREGIST_FROM_SERVER;
            break;
        case ID_MOUSE_SERVER:
            header = enabled ? MSG_MOUSE_REGIST_TO_SERVER : MSG_MOUSE_UNREGIST_FROM_SERVER;
            break;
        default:
            header = enabled ? MSG_REGISTER_TO_SERVER : MSG_UNREGISTER_FROM_SERVER;
            break;
    }
    if (tid == THREAD_UNKNOWN) return 0;

    if (monapi_cmessage_send_receive_args(NULL, tid, header, syscall_get_tid(), 0, 0, NULL) != 0)
    {
        printf("ERROR: can not register to %s\n", server_names[id]);
        return 0;
    }
    return 1;
}

int monapi_call_mouse_set_cursor(int enabled)
{
    dword tid = monapi_get_server_thread_id(ID_MOUSE_SERVER);
    dword header = enabled ? MSG_MOUSE_ENABLE_CURSOR : MSG_MOUSE_DISABLE_CURSOR;
    MessageInfo msg;
    if (monapi_cmessage_send_receive_args(&msg, tid, header, 0, 0, 0, NULL) != 0)
    {
        return 0;
    }
    return 1;
}

monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, int prompt)
{
    monapi_cmemoryinfo* ret;
    dword tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (monapi_cmessage_send_receive_args(&msg, tid, MSG_FILE_READ_DATA, prompt, 0, 0, file) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;

    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = tid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi)
{
    monapi_cmemoryinfo* ret;
    dword tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (monapi_cmessage_send_receive_args(&msg, tid, MSG_FILE_DECOMPRESS_BZ2, mi->Handle, mi->Size, 0, NULL) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;

    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = tid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, int prompt)
{
    monapi_cmemoryinfo* ret;
    dword tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (monapi_cmessage_send_receive_args(&msg, tid, MSG_FILE_DECOMPRESS_BZ2_FILE, prompt, 0, 0, file) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;

    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = tid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

int monapi_call_elf_execute_file(const char* command_line, int prompt)
{
    return monapi_call_elf_execute_file_get_tid(command_line, prompt, NULL);
}

int monapi_call_elf_execute_file_get_tid(const char* command_line, int prompt, dword* tid)
{
    dword svr = monapi_get_server_thread_id(ID_ELF_SERVER);
    MessageInfo msg;
    if (monapi_cmessage_send_receive_args(&msg, svr, MSG_ELF_EXECUTE_FILE, prompt, 0, 0, command_line) != 0)
    {
        if (tid != NULL) *tid = THREAD_UNKNOWN;
        return -1;
    }
    if (tid != NULL) *tid = msg.arg3;
    return msg.arg2;
}

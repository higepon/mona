#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/cmessage.h>

static dword server_ids[] =
{
    THREAD_UNKNOWN,  // ID_MOUSE_SERVER
    THREAD_UNKNOWN,  // ID_KEYBOARD_SERVER
    THREAD_UNKNOWN,  // ID_FILE_SERVER
    THREAD_UNKNOWN   // ID_GUI_SERVER
};

static const char* server_names[] =
{
    "MOUSE.SVR", "KEYBDMNG.SVR", "FILE.SVR", "GUI.SVR"
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

int monapi_register_to_server(int id, int enabled)
{
    dword tid = THREAD_UNKNOWN, header = MSG_NONE;
    switch (id)
    {
        case ID_KEYBOARD_SERVER:
            tid = monapi_get_server_thread_id(ID_KEYBOARD_SERVER);
            header = enabled ? MSG_KEY_REGIST_TO_SERVER : MSG_KEY_UNREGIST_FROM_SERVER;
            break;
        case ID_MOUSE_SERVER:
            tid = monapi_get_server_thread_id(ID_MOUSE_SERVER);
            header = enabled ? MSG_MOUSE_REGIST_TO_SERVER : MSG_MOUSE_UNREGIST_FROM_SERVER;
            break;
    }
    if (tid == THREAD_UNKNOWN) return 0;

    if (monapi_cmessage_send_args(tid, header, syscall_get_tid(), 0, 0, NULL) != 0)
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
    dword tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (monapi_cmessage_send_receive_args(&msg, tid, MSG_FILE_READ_DATA, prompt, 0, 0, file) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == 0) return NULL;

    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

#ifndef __MONAPI_MESSAGES_H__
#define __MONAPI_MESSAGES_H__

#include <monapi/cmemoryinfo.h>

enum
{
    MSG_DISPOSE_HANDLE = 0x0100,

    MSG_KEY_REGIST_TO_SERVER = 0x0200,
    MSG_KEY_UNREGIST_FROM_SERVER,
    MSG_KEY_VIRTUAL_CODE,

    MSG_MOUSE_REGIST_TO_SERVER = 0x0300,
    MSG_MOUSE_UNREGIST_FROM_SERVER,
    MSG_MOUSE_INFO,
    MSG_MOUSE_ENABLE_CURSOR,
    MSG_MOUSE_DISABLE_CURSOR,

    MSG_FILE_READ_DATA = 0x0400
};

enum
{
    ID_MOUSE_SERVER,
    ID_KEYBOARD_SERVER,
    ID_FILE_SERVER,
    ID_GUI_SERVER,
    ID_NUMBER_OF_SERVERS
};

#ifdef __cplusplus
extern "C"
{
#endif
extern dword monapi_get_server_thread_id(int id);
extern int monapi_call_dispose_handle(int id, dword handle);
extern int monapi_register_to_server(int id, int enabled);
extern int monapi_call_mouse_set_cursor(int enabled);
extern monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, int prompt);
#ifdef __cplusplus
}
#endif

#endif  // __MONAPI_MESSAGES_H__

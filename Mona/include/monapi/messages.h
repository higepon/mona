#ifndef __MONAPI_MESSAGES_H__
#define __MONAPI_MESSAGES_H__

#include <monapi/clist.h>
#include <monapi/cmessage.h>
#include <monapi/cmemoryinfo.h>

enum
{
    MSG_DISPOSE_HANDLE = 0x0100,

    MSG_REGISTER_TO_SERVER = 0x0110,
    MSG_UNREGISTER_FROM_SERVER,

    MSG_KEY_REGIST_TO_SERVER = 0x0200,
    MSG_KEY_UNREGIST_FROM_SERVER,
    MSG_KEY_VIRTUAL_CODE,

    MSG_MOUSE_REGIST_TO_SERVER = 0x0300,
    MSG_MOUSE_UNREGIST_FROM_SERVER,
    MSG_MOUSE_INFO,
    MSG_MOUSE_ENABLE_CURSOR,
    MSG_MOUSE_DISABLE_CURSOR,

    MSG_FILE_READ_DATA = 0x0400,
    MSG_FILE_DECOMPRESS_BZ2,
    MSG_FILE_DECOMPRESS_BZ2_FILE,

    MSG_ELF_EXECUTE_FILE = 0x0500,

    MSG_PROCESS_GET_PROCESS_INFO = 0x1000,
    MSG_PROCESS_CREATED,
    MSG_PROCESS_TERMINATED,

    MSG_PROCESS_GRAB_STDOUT = 0x10f0,  // temporary
    MSG_PROCESS_UNGRAB_STDOUT,         // temporary
    MSG_PROCESS_STDOUT_DATA            // temporary
};

enum
{
    ID_MOUSE_SERVER,
    ID_KEYBOARD_SERVER,
    ID_FILE_SERVER,
    ID_GUI_SERVER,
    ID_ELF_SERVER,
    ID_PROCESS_SERVER,
    ID_NUMBER_OF_SERVERS
};

#define PROCESS_STDOUT_THREAD (monapi_get_server_thread_id(ID_PROCESS_SERVER) + 1)  // temporary

#ifdef __cplusplus
extern "C"
{
#endif
extern dword monapi_get_server_thread_id(int id);
extern MONAPI_BOOL monapi_call_dispose_handle(int id, dword handle);
extern MONAPI_BOOL monapi_register_to_server(int id, MONAPI_BOOL enabled);
extern MONAPI_BOOL monapi_call_mouse_set_cursor(monapi_clist* queue, MONAPI_BOOL enabled);
extern monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt);
extern int monapi_call_elf_execute_file(const char* command_line, MONAPI_BOOL prompt);
extern int monapi_call_elf_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, dword* tid);
#ifdef __cplusplus
}
#endif

#endif  /* __MONAPI_MESSAGES_H__ */

#ifndef __MONAPI_MESSAGES_H__
#define __MONAPI_MESSAGES_H__

#include <monapi/cmemoryinfo.h>

#define MONAPI_BOOL int
#define MONAPI_TRUE   1
#define MONAPI_FALSE  0

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
    MSG_MOUSE_GET_CURSOR_POSITION,

    MSG_FILE_READ_DATA = 0x0400,
    MSG_FILE_CHANGE_DRIVE,
    MSG_FILE_GET_CURRENT_DRIVE,
    MSG_FILE_CHANGE_DIRECTORY,
    MSG_FILE_GET_CURRENT_DIRECTORY,
    MSG_FILE_DECOMPRESS_BZ2,
    MSG_FILE_DECOMPRESS_BZ2_FILE,
    MSG_FILE_DECOMPRESS_ST5,
    MSG_FILE_DECOMPRESS_ST5_FILE,
    MSG_FILE_READ_DIRECTORY = 0x0410,

    MSG_PROCESS_EXECUTE_FILE = 0x0500,
    MSG_PROCESS_CREATE_IMAGE,
    MSG_PROCESS_GET_PROCESS_INFO = 0x1000,
    MSG_PROCESS_CREATED,
    MSG_PROCESS_TERMINATED,
    MSG_PROCESS_GET_COMMON_PARAMS,
    MSG_PROCESS_GRAB_STDOUT = 0x10f0,  // temporary
    MSG_PROCESS_UNGRAB_STDOUT,         // temporary
    MSG_PROCESS_STDOUT_DATA,           // temporary
};

enum
{
    ID_MOUSE_SERVER,
    ID_KEYBOARD_SERVER,
    ID_FILE_SERVER,
    ID_GUI_SERVER,
    ID_ELF_SERVER,
    ID_PROCESS_SERVER,
    ID_PE_SERVER,
    ID_MONITOR_SERVER,
    ID_NUMBER_OF_SERVERS
};

typedef struct
{
    char name[16];
    int size, attr;
} monapi_directoryinfo;

typedef struct
{
    struct
    {
        int x, y, buttons;
    } mouse;
} CommonParameters;

#ifdef __cplusplus
extern "C"
{
#endif
extern dword monapi_get_server_thread_id(int id);
extern MONAPI_BOOL monapi_call_dispose_handle(int id, dword handle);
extern MONAPI_BOOL monapi_register_to_server(int id, MONAPI_BOOL enabled);
extern MONAPI_BOOL monapi_call_mouse_set_cursor(MONAPI_BOOL enabled);
extern monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5_file(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_read_directory(const char* path, MONAPI_BOOL prompt);
extern int monapi_call_process_execute_file(const char* command_line, MONAPI_BOOL prompt);
extern int monapi_call_process_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, dword* tid, dword stdout_id = NULL);
extern int monapi_call_change_drive(int drive, MONAPI_BOOL prompt);
extern int monapi_call_get_current_drive();
extern int monapi_call_get_current_directory(char* dest);
extern int monapi_call_change_directory(const char* dest);

extern byte* monapi_allocate_dma_memory();
extern void monapi_deallocate_dma_memory(void* address);

#ifdef __cplusplus
}
#endif

#endif  /* __MONAPI_MESSAGES_H__ */

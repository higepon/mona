#ifndef __MONAPI_MESSAGES_H__
#define __MONAPI_MESSAGES_H__

#ifndef ON_LINUX
#include <monapi/cmemoryinfo.h>
#include <servers/keyboard.h>
#include <servers/mouse.h>
#include <servers/process.h>
#include <servers/file.h>
#include <servers/servers.h>
#else
#include <monapi/cmemoryinfo.h>
#include "types.h"
#endif

#define MONAPI_BOOL int
#define MONAPI_TRUE   1
#define MONAPI_FALSE  0

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
extern dword monapi_call_file_open(const char* file);
extern dword monapi_call_file_open2(const char* file);
extern monapi_cmemoryinfo* monapi_call_file_read_data2(dword fileID, dword size);
extern int monapi_call_file_seek2(dword fileID, dword offset, dword origin);
extern dword monapi_call_file_get_file_size(dword id);
extern MONAPI_BOOL monapi_call_file_seek(dword id, dword position, dword flag);
extern MONAPI_BOOL monapi_call_file_close(dword id);
extern MONAPI_BOOL monapi_call_file_close2(dword id);
extern monapi_cmemoryinfo* monapi_call_file_read(dword id, dword size);
extern monapi_cmemoryinfo* monapi_call_file_read_data(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5_file(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_read_directory(const char* path, MONAPI_BOOL prompt);
extern int monapi_call_process_execute_file(const char* command_line, MONAPI_BOOL prompt);
extern int monapi_call_process_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, dword* tid, dword stdout_id);
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

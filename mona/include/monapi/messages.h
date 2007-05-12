#ifndef _MONAPI_MESSAGES_
#define _MONAPI_MESSAGES_

#include <monapi/cmemoryinfo.h>
#include <servers/keyboard.h>
#include <servers/mouse.h>
#include <servers/process.h>
#include <servers/file.h>
#include <servers/servers.h>

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
    ID_SCHEME_SERVER,
    ID_SCREEN_SERVER,
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
extern uint32_t monapi_get_server_thread_id(int id);
extern MONAPI_BOOL monapi_call_dispose_handle(int id, uint32_t handle);
extern MONAPI_BOOL monapi_register_to_server(int id, MONAPI_BOOL enabled);
extern MONAPI_BOOL monapi_call_mouse_set_cursor(MONAPI_BOOL enabled);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5_file(const char* file, MONAPI_BOOL prompt);
extern int monapi_call_process_execute_file(const char* command_line, MONAPI_BOOL prompt);
extern int monapi_call_process_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, uint32_t* tid, uint32_t stdout_id, uint32_t stdin_id);
extern int monapi_call_change_drive(int drive, MONAPI_BOOL prompt);
extern uint32_t monapi_file_open(const char* file, MONAPI_BOOL create);
extern uint32_t monapi_file_write(uint32_t fileID, monapi_cmemoryinfo* mem, uint32_t size);
extern monapi_cmemoryinfo* monapi_file_read(uint32_t fileID, uint32_t size);
extern uint32_t monapi_file_seek(uint32_t fileID, uint32_t offset, uint32_t origin);
extern uint32_t monapi_file_close(uint32_t id);
extern uint32_t monapi_file_get_file_size(uint32_t id);
extern int monapi_file_stop_server();
extern monapi_cmemoryinfo* monapi_file_read_all(const char* file);
extern monapi_cmemoryinfo* monapi_file_read_directory(const char* path);
extern uint32_t monapi_stdin_read(uint8_t* buffer, uint32_t size);
extern uint32_t monapi_stdin_lock_for_read();
extern uint32_t monapi_stdin_unlock_for_read();
extern uint32_t monapi_stdin_try_lock_for_read();
extern uint32_t monapi_stdout_write(uint8_t* buffer, uint32_t size);
extern uint8_t* monapi_allocate_dma_memory(int size);
extern void monapi_deallocate_dma_memory(void* address, int size);
extern MONAPI_BOOL monapi_notify_server_start(const char*);
extern uint32_t monapi_process_wait_terminated(uint32_t tid);

#ifdef __cplusplus
}
#endif

#endif  /* __MONAPI_MESSAGES_H__ */

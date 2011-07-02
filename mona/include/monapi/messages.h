#ifndef _MONAPI_MESSAGES_
#define _MONAPI_MESSAGES_

#ifdef __cplusplus
#include <monapi/MemoryMap.h>
#include <monapi/SharedMemory.h>
#include <monapi/Date.h>
#endif
#include <servers/keyboard.h>
#include <servers/mouse.h>
#include <servers/process.h>
#include <servers/file.h>
#include <servers/servers.h>
#include <servers/clipboard.h>

#define MONAPI_BOOL int
#define MONAPI_TRUE   1
#define MONAPI_FALSE  0

// enum
// {
//     ID_MOUSE_SERVER,
//     ID_KEYBOARD_SERVER,
//     ID_FILE_SERVER,
//     ID_GUI_SERVER,
//     ID_ELF_SERVER,
//     ID_PROCESS_SERVER,
//     ID_PE_SERVER,
//     ID_MONITOR_SERVER,
//     ID_SCHEME_SERVER,
// //    ID_SCREEN_SERVER,
//     ID_NET_SERVER,
//     ID_CLIPBOARD_SERVER,
//     ID_NUMBER_OF_SERVERS
// };

enum FileOpenMode {
    FILE_CREATE   = 0x00000010,
    FILE_TRUNCATE = 0x00000020,
    FORBIT_EXTRA_COMMA
};

typedef struct
{
    char name[128]; // Joliet long name
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
class MonAPI::SharedMemory;
class MonAPI::Date;
extern intptr_t monapi_name_clear_cache(const char* name);
extern intptr_t monapi_name_add(const char* name);
extern intptr_t monapi_name_whereis(const char* name, uint32_t& id);
extern intptr_t monapi_name_get_server(uint32_t& id);
extern intptr_t monapi_file_write(uint32_t fileID, const MonAPI::SharedMemory& mem, uint32_t size);
extern MonAPI::SharedMemory* monapi_file_read(uint32_t fileID, uint32_t size);
extern MonAPI::SharedMemory* monapi_file_read_all(const char* file);
extern MonAPI::SharedMemory* monapi_file_read_directory(const char* path);
extern MonAPI::SharedMemory* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt);
extern MonAPI::SharedMemory* monapi_call_file_decompress_st5_file(const char* file, MONAPI_BOOL prompt);
extern MonAPI::SharedMemory* monapi_call_file_decompress_st5(const MonAPI::SharedMemory& shm);
extern MonAPI::SharedMemory* monapi_call_file_decompress_bz2(const MonAPI::SharedMemory& shm);
extern intptr_t monapi_file_get_date(const char* file, MonAPI::Date& dest);
extern intptr_t monapi_file_get_file_size_by_path(const char* file, uint32_t& size);
extern intptr_t monapi_clipboard_set(const MonAPI::SharedMemory& shm);
extern MonAPI::SharedMemory* monapi_clipboard_get();
#endif
#ifdef __cplusplus
extern "C"
{
#endif
extern intptr_t monapi_register_to_server(const char* server);
extern intptr_t monapi_unregister_to_server(const char* server);
extern MONAPI_BOOL monapi_call_mouse_set_cursor(MONAPI_BOOL enabled);

extern intptr_t monapi_enable_stacktrace(const char* mapFilePath);
extern intptr_t monapi_process_register_thread(uint32_t tid);
extern intptr_t monapi_process_execute_file(const char* command_line, MONAPI_BOOL prompt);
extern intptr_t monapi_process_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, uint32_t* tid, uint32_t stdout_id, uint32_t stdin_id);
extern intptr_t monapi_file_open(const char* file, intptr_t mode);
extern intptr_t monapi_file_seek(uint32_t fileID, int32_t offset, uint32_t origin);
extern intptr_t monapi_file_close(uint32_t id);
extern intptr_t monapi_file_get_file_size(uint32_t id);
extern intptr_t monapi_file_delete(const char* file);
extern intptr_t monapi_file_create_directory(const char* file);
extern intptr_t monapi_file_delete_directory(const char* file);
extern MONAPI_BOOL monapi_file_exists(const char* path);
extern MONAPI_BOOL monapi_file_is_directory(const char* path);
extern intptr_t monapi_file_stop_server();
extern uint32_t monapi_stdin_read(uint8_t* buffer, uint32_t size);
extern uint32_t monapi_stdin_lock_for_read();
extern uint32_t monapi_stdin_unlock_for_read();
extern uint32_t monapi_stdin_try_lock_for_read();
extern intptr_t monapi_stdout_write(uint8_t* buffer, uint32_t size);
extern uint8_t* monapi_allocate_dma_memory(int size);
extern void monapi_deallocate_dma_memory(void* address, int size);
extern intptr_t monapi_notify_server_start(const char*);
extern intptr_t monapi_process_wait_terminated(uint32_t tid);
extern const char* monapi_error_string(int error);
extern intptr_t monapi_clipboard_clear();

typedef struct malloc_stat
{
    uintptr_t max_system;
    uintptr_t system;
    uintptr_t used;
} malloc_stat;

void malloc_stats(struct malloc_stat* st); // for malloc debug

#define MALLOC_STATS() { struct malloc_stat st; malloc_stats(&st); logprintf("used %x %s:%d\n", st.used, __FILE__, __LINE__); }

#ifdef __cplusplus
}
#endif

#endif  /* __MONAPI_MESSAGES_H__ */

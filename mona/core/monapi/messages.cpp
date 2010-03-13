#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/Message.h>
#include <monapi/Stream.h>
#include <monapi/System.h>
#include <monapi/string.h>

using namespace MonAPI;

static uint32_t server_ids[] =
{
    THREAD_UNKNOWN,  // ID_MOUSE_SERVER
    THREAD_UNKNOWN,  // ID_KEYBOARD_SERVER
    THREAD_UNKNOWN,  // ID_FILE_SERVER
    THREAD_UNKNOWN,  // ID_GUI_SERVER
    THREAD_UNKNOWN,  // ID_ELF_SERVER
    THREAD_UNKNOWN,  // ID_PROCESS_SERVER
    THREAD_UNKNOWN,  // ID_PE_SERVER
    THREAD_UNKNOWN,  // ID_MONITOR_SERVER
    THREAD_UNKNOWN,  // ID_SCHEME_SERVER
    THREAD_UNKNOWN   // ID_NET_SERVER
};

static const char* server_names[] =
{
    "MOUSE.EX5", "KEYBDMNG.EX5", "FILE.BIN", "GUI.EX5", "ELF.BN5", "PROCESS.BIN", "PE.BN5", "MONITOR.BIN", "SCHEME.EX5", "NET.EX5"
};

uint32_t monapi_get_server_thread_id(int id)
{
    if (id < 0 || ID_NUMBER_OF_SERVERS <= id) return THREAD_UNKNOWN;

    if (server_ids[id] == THREAD_UNKNOWN)
    {
        server_ids[id] = Message::lookupMainThread(server_names[id]);
        if (server_ids[id] == THREAD_UNKNOWN)
        {
           MONAPI_WARN("ERROR: can not connect to %s", server_names[id]);
        }

        // quick hack.
        if (id == ID_NET_SERVER) {
            server_ids[id] = server_ids[id] + 2;
        }
    }
    return server_ids[id];
}

MONAPI_BOOL monapi_call_dispose_handle(int id, uint32_t handle)
{
    uint32_t tid = monapi_get_server_thread_id(ID_MOUSE_SERVER);
    if (Message::send(tid, MSG_DISPOSE_HANDLE, handle, 0, 0, NULL) != 0)
    {
        return MONAPI_FALSE;
    }
    return MONAPI_TRUE;
}

MONAPI_BOOL monapi_register_to_server(int id, MONAPI_BOOL enabled)
{
    uint32_t tid = monapi_get_server_thread_id(id), header = MSG_NONE;
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
    if (tid == THREAD_UNKNOWN) return MONAPI_FALSE;

    if (Message::sendReceive(NULL, tid, header, syscall_get_tid()) != 0)
    {
        MONAPI_WARN("ERROR: can not register to %s", server_names[id]);
        return MONAPI_FALSE;
    }
    return MONAPI_TRUE;
}

MONAPI_BOOL monapi_call_mouse_set_cursor(MONAPI_BOOL enabled)
{
    uint32_t tid = monapi_get_server_thread_id(ID_MOUSE_SERVER);
    uint32_t header = enabled ? MSG_MOUSE_ENABLE_CURSOR : MSG_MOUSE_DISABLE_CURSOR;
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, header) != 0)
    {
        return MONAPI_FALSE;
    }
    return MONAPI_TRUE;
}


monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_DECOMPRESS_BZ2, mi->Handle, mi->Size) != 0)
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

monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);

    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_DECOMPRESS_BZ2_FILE, prompt, 0, 0, file) != 0)
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

monapi_cmemoryinfo* monapi_call_file_decompress_st5(monapi_cmemoryinfo* mi)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_DECOMPRESS_ST5, mi->Handle, mi->Size) != 0)
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

monapi_cmemoryinfo* monapi_call_file_decompress_st5_file(const char* file, MONAPI_BOOL prompt)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);

    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_DECOMPRESS_ST5_FILE, prompt, 0, 0, file) != 0)
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

#if 0
monapi_cmemoryinfo* monapi_call_file_read_directory(const char* path, MONAPI_BOOL prompt)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_READ_DIRECTORY, prompt, 0, 0, path) != 0)
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
#endif

int monapi_call_process_execute_file(const char* command_line, MONAPI_BOOL prompt)
{
    return monapi_call_process_execute_file_get_tid(command_line, prompt, NULL, NULL, NULL);
}

int monapi_call_process_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, uint32_t* tid, uint32_t stdin_id /* = NULL */, uint32_t stdout_id /* NULL */)
{
    uint32_t svr = monapi_get_server_thread_id(ID_PROCESS_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, svr, MSG_PROCESS_EXECUTE_FILE, prompt, stdin_id, stdout_id, command_line) != 0)
    {
        if (tid != NULL) *tid = THREAD_UNKNOWN;
        return -1;
    }
    if (tid != NULL) *tid = msg.arg3;
    return msg.arg2;
}

// size should be 4096 * x
uint8_t* monapi_allocate_dma_memory(int size)
{
    return syscall_allocate_dma_memory(size);
}

// size should be 4096 * x
void monapi_deallocate_dma_memory(void* address, int size)
{
    syscall_deallocate_dma_memory(address, size);
}

monapi_cmemoryinfo* monapi_file_read_all(const char* file)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_READ_ALL, 0, 0, 0, file) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == MONA_FAILURE) { return NULL;}

    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = tid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

uint32_t monapi_file_open(const char* file, MONAPI_BOOL create)
{
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_OPEN, create, 0, 0, file) != 0)
    {
        return MONA_FAILURE;
    }
    return msg.arg2;
}

monapi_cmemoryinfo* monapi_file_read(uint32_t fileID, uint32_t size)
{
    static int count = 0;
    count++;
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;

    if (Message::sendReceive(&msg, tid, MSG_FILE_READ, fileID, size) != 0)
    {
        return NULL;
    }
    if (msg.arg2 == MONA_FAILURE)
    {
        return NULL;
    }
    ret = monapi_cmemoryinfo_new();
    ret->Handle = msg.arg2;
    ret->Owner  = tid;
    ret->Size   = msg.arg3;
    monapi_cmemoryinfo_map(ret);
    return ret;
}

uint32_t monapi_file_seek(uint32_t fileID, uint32_t offset, uint32_t origin)
{
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_SEEK, fileID, offset, origin) != 0)
    {
        return MONA_FAILURE;
    }
    return msg.arg2;
}

uint32_t monapi_file_close(uint32_t fileID)
{
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_CLOSE, fileID) != 0)
    {
        return MONA_FAILURE;
    }
    return msg.arg2;
}

monapi_cmemoryinfo* monapi_file_read_directory(const char* path)
{
    monapi_cmemoryinfo* ret;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_READ_DIRECTORY, 0, 0, 0, path) != 0)
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

uint32_t monapi_file_get_file_size(uint32_t id)
{
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);

    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_FILE_GET_SIZE, id) != 0)
    {
        return MONA_FAILURE;
    }
    return msg.arg3;
}

int monapi_file_stop_server()
{
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    MessageInfo msg;
    if (Message::sendReceive(&msg, tid, MSG_STOP_SERVER) != 0)
    {
        return MONA_FAILURE;
    }
    return msg.arg2;
}

uint32_t monapi_file_write(uint32_t fileID, monapi_cmemoryinfo* mem, uint32_t size)
{
    MessageInfo msg;
    uint32_t tid = monapi_get_server_thread_id(ID_FILE_SERVER);
    if (Message::sendReceive(&msg, tid, MSG_FILE_WRITE, fileID, size, mem->Handle) != 0)
    {
        return MONA_FAILURE;
    }
    return msg.arg2;
}

extern Stream* inStream;
extern Stream* outStream;

uint32_t monapi_stdin_read(uint8_t* buffer, uint32_t size)
{
    System::getStdinStream();
    inStream->waitForRead();
    return inStream->read(buffer, size);
}

uint32_t monapi_stdout_write(uint8_t* buffer, uint32_t size)
{
    System::getStdoutStream();
    if (NULL == outStream)
    {
       MONAPI_WARN("%s You can't use printf, use _printf instead.", System::getProcessInfo()->name);
        _printf("Because you process is executed from monitor server, so you have no stdout\n at %s %s:%d\n", __func__, __FILE__, __LINE__);
        return 0;
    }
    return outStream->write(buffer, size);
}

uint32_t monapi_stdin_lock_for_read()
{
    System::getStdinStream();
    return inStream->lockForRead();
}

uint32_t monapi_stdin_try_lock_for_read()
{
    System::getStdinStream();
    return inStream->tryLockForRead();
}

uint32_t monapi_stdin_unlock_for_read()
{
    System::getStdinStream();
    return inStream->unlockForRead();
}

uint32_t monapi_process_wait_terminated(uint32_t tid)
{
    for (MessageInfo msg;;)
    {
        if (MonAPI::Message::receive(&msg) != 0) continue;
        switch (msg.header)
        {
        case MSG_PROCESS_TERMINATED:
            if (tid == msg.arg1)
            {
                // exit status code
                return msg.arg2;
            }
            break;
        default:
            break;
        }
    }
    return MONA_FAILURE;
}

intptr_t monapi_notify_server_start(const char* name)
{
    uint32_t targetID = Message::lookupMainThread(name);

    if (targetID == THREAD_UNKNOWN)
    {
        MONAPI_WARN("%s:INIT not found", name);
        return M_NAME_NOT_FOUND;
    }

    /* send */
    if(Message::send(targetID, MSG_SERVER_START_OK))
    {
        MONAPI_WARN("%s:INIT error", name);
        return M_NAME_NOT_FOUND;
    }
    return M_OK;
}

#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monapi/Array.h>
#include <monalibc.h>
#include "ProcessServer.h"
#include "ProcessManager.h"

using namespace MonAPI;

static int ExecuteProcess(uint32_t parent, monapi_cmemoryinfo* mi, uint32_t entryPoint, const CString& path, const CString& name, CommandOption* option, bool prompt, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid)
{
    LoadProcessInfo info;
    info.image = mi->Data;
    info.size  = mi->Size;
    info.entrypoint = entryPoint;
    info.path = path;
    info.name = name;
    info.list = option;

    addProcessInfo(name);
    int ret = syscall_load_process_image(&info);
    *tid = addProcessInfo(parent, name, path, stdin_id, stdout_id);

    if (prompt)
    {
        switch(ret)
        {
            case 4:
                  printf("%s: Shared Memory error1", SVR);
                  break;
            case 5:
                  printf("%s: Shared Memory error2", SVR);
                  break;
        }
    }

    return ret;
}

static CString GetFileName(const CString& path)
{
    int p = path.lastIndexOf('/');
    if (p < 0) return path;

    p++;
    return path.substring(p, path.getLength() - p);
}

static int ExecuteFile(uint32_t parent, const CString& commandLine, bool prompt, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid)
{
    /* list initilize */
    CommandOption list;
    list.next = NULL;
    CommandOption* option = NULL;
    CString path;
    _A<CString> args = commandLine.split(' ');
    FOREACH (CString, arg, args)
    {
        if (arg == NULL) continue;

        if (path == NULL)
        {
            path = arg.toUpper();
            continue;
        }
        option = new CommandOption;
        strncpy(option->str, arg, sizeof(option->str));
        option->next = list.next;
        list.next = option;
    }
    END_FOREACH

    monapi_cmemoryinfo* mi = NULL;
    uint32_t entryPoint = 0xa0000000;
    int result = 1, svr_id = -1;

    if (path.endsWith(".ELF") || path.endsWith(".EL2") || path.endsWith(".EL5"))
    {
        svr_id = ID_ELF_SERVER;
    }
    else if (path.endsWith(".EXE") || path.endsWith(".EX2") || path.endsWith(".EX5"))
    {
        svr_id = ID_PE_SERVER;
    }
    if (svr_id != -1)
    {
        MessageInfo msg;
        uint32_t tid = monapi_get_server_thread_id(svr_id);

        if (tid != THREAD_UNKNOWN)
        {
            Message::sendReceive(&msg, tid, MSG_PROCESS_CREATE_IMAGE, prompt ? MONAPI_TRUE : MONAPI_FALSE, 0, 0, path);
            if (msg.arg2 != 0)
            {
                result = 0;
                entryPoint = msg.arg3;
                mi = monapi_cmemoryinfo_new();
                mi->Handle = msg.arg2;
                mi->Owner  = tid;
                mi->Size   = atoi(msg.str);
                monapi_cmemoryinfo_map(mi);
            }
            else
            {
                result = msg.arg3;
            }
        }
    }
    else if (path.endsWith(".BN2"))
    {
        mi = monapi_call_file_decompress_bz2_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else if (path.endsWith(".BN5"))
    {
        mi = monapi_call_file_decompress_st5_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else
    {
        mi = monapi_file_read_all(path);
    }

    if (mi == NULL)
    {
        return result;
    }
    else
    {
        result = ExecuteProcess(parent, mi, entryPoint, path, GetFileName(path), &list, prompt, stdin_id, stdout_id, tid);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }
    CommandOption* next;
    for (option = list.next; option; option = next)
    {
        next = option->next;
        delete option;
    }
    return result;
}

static void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_PROCESS_EXECUTE_FILE:
            {
                uint32_t tid;
                int result = ExecuteFile(msg.from, msg.str, msg.arg1 != 0, msg.arg2, msg.arg3, &tid);
                Message::reply(&msg, result, tid);
                break;
            }
            default:
                if (processHandler(&msg)) break;
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    initCommonParameters();

    if (MONAPI_FALSE == monapi_notify_server_start("INIT"))
    {
        exit(-1);
    }

    MessageLoop();

    return 0;
}

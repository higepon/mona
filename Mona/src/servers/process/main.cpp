#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <gui/System/Array.h>
#include "ProcessServer.h"
#include "ProcessManager.h"

using namespace MonAPI;

static int ExecuteProcess(dword parent, monapi_cmemoryinfo* mi, dword entryPoint, const CString& path, const CString& name, CommandOption* option, bool prompt, dword stdout_id, dword* tid)
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
    *tid = addProcessInfo(parent, name, path);

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

    registerStdout(*tid, stdout_id);

    return ret;
}

static CString GetFileName(const CString& path)
{
    int p = path.lastIndexOf('/');
    if (p < 0) return path;

    p++;
    return path.substring(p, path.getLength() - p);
}

static int ExecuteFile(dword parent, const CString& commandLine, bool prompt, dword stdout_id, dword* tid)
{
    Log("Execute=%s\n", (const char*)commandLine);

    /* list initilize */
    CommandOption list;
    list.next = NULL;
    Log("\n");
    CommandOption* option = NULL;
    CString path;
    _A<CString> args = commandLine.split(' ');

    Log("\n");
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
    Log("\n");
    monapi_cmemoryinfo* mi = NULL;
    dword entryPoint = 0xa0000000;
    int result = 1, svr_id = -1;

    Log("\n");

    if (path.endsWith(".ELF") || path.endsWith(".EL2") || path.endsWith(".EL5"))
    {
    Log("\n");
        svr_id = ID_ELF_SERVER;
    }
    else if (path.endsWith(".EXE") || path.endsWith(".EX2") || path.endsWith(".EX5"))
    {
    Log("\n");
        svr_id = ID_PE_SERVER;
    }
    if (svr_id != -1)
    {
    Log("\n");
        MessageInfo msg;
        dword tid = monapi_get_server_thread_id(svr_id);

        if (tid != THREAD_UNKNOWN)
        {
    Log("\n");
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
    Log("\n");
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
        mi = monapi_call_file_read_data(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }

    if (mi == NULL)
    {
        if (prompt) printf("%s: can not execute!\n", SVR);
    }
    else
    {
    Log("\n");
        result = ExecuteProcess(parent, mi, entryPoint, path, GetFileName(path), &list, prompt, stdout_id, tid);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

    for (option = list.next; option; option = option->next)
    {
        delete option;
    }
    return result;
}

#if 1  // temporary
HList<dword> grabs;

static void StdoutGrab(dword tid)
{
    int size = grabs.size();
    bool ok = true;
    for (int i = 0; i < size; i++)
    {
        if (grabs[i] == tid)
        {
            ok = false;
            break;
        }
    }
    if (ok) grabs.add(tid);
}

static void StdoutUngrab(dword tid)
{
    int size = grabs.size();
    for (int i = 0; i < size; i++)
    {
        if (grabs[i] == tid)
        {
            grabs.removeAt(i);
            return;
        }
    }
}

static void StdoutMessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_PROCESS_STDOUT_DATA:
            {
#if 0  /// DEBUG for message
                char buf[128];
                sprintf(buf, "?%d?", msg.from);
                syscall_print(buf);
#endif
                int size;
                bool ok = false;
                while ((size = grabs.size()) > 0)
                {
                    if (Message::sendReceive(NULL, grabs[size - 1], MSG_PROCESS_STDOUT_DATA, 0, 0, 0, msg.str) == 0)
                    {
                        ok = true;
                        break;
                    }
                    StdoutUngrab(grabs[size - 1]);
                }
                if (!ok) syscall_print(msg.str);
#if 0  /// DEBUG for message
                syscall_print("?E?");
#endif
                Message::reply(&msg);
                break;
            }
            case MSG_PROCESS_GRAB_STDOUT:
                StdoutGrab(msg.arg1);
                Message::reply(&msg);
                break;
            case MSG_PROCESS_UNGRAB_STDOUT:
                StdoutUngrab(msg.arg1);
                Message::reply(&msg);
                break;
        }
    }
}
#endif

static void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

#if 0  /// DEBUG for message
        if ((msg.header == MSG_RESULT_OK && msg.arg1 == MSG_PROCESS_STDOUT_DATA) || msg.header == MSG_PROCESS_STDOUT_DATA)
        {
            char buf[128];
            sprintf(buf, "**** INVALID MESSAGE!! ****[%d: %d, %d]\n", syscall_get_tid(), msg.header, msg.arg1);
            syscall_print(buf);
            for (;;);
        }
#endif
        switch (msg.header)
        {
            case MSG_PROCESS_EXECUTE_FILE:
            {
    Log("\n");
                dword tid;

#if 1 // exp
		// override stdout_id here by higepon
		// because Tino GUI's GUI SHELL does not call execute process by himself but shell svr
		if (grabs.size() != 0)
		{
		    msg.arg2 = grabs[grabs.size() - 1];
		}
#endif
    Log("\n");
                int result = ExecuteFile(msg.from, msg.str, msg.arg1 != 0, msg.arg2, &tid);
    Log("\n");
                Message::reply(&msg, result, tid);
                break;
            }
            default:
                if (processHandler(&msg)) break;
                break;
        }
    }
}

int MonaMain(List<char*>* pekoe)
{
    initCommonParameters();
#if 1  // temporary
    dword id = syscall_mthread_create((dword)StdoutMessageLoop);
    syscall_mthread_join(id);
#endif

    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }

    MessageLoop();

    return 0;
}

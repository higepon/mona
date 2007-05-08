#include "Shell.h"
#include <monapi/messages.h>
#include <monalibc.h>

using namespace MonAPI;

enum
{
    COMMAND_NONE,
    COMMAND_HELP,
    COMMAND_LS,
    COMMAND_CD,
    COMMAND_CAT,
    COMMAND_UNAME,
    COMMAND_ECHO,
    COMMAND_CLEAR,
    COMMAND_PS,
    COMMAND_MEM,
    COMMAND_KILL,
    COMMAND_EXEC,
    COMMAND_SHUTDOWN,
    COMMAND_HALT,
    COMMAND_REBOOT,
};

int Shell::isInternalCommand(const CString& command)
{
    CString cmd = command.toLower();
    if (cmd == "help" || cmd == "?")
    {
        return COMMAND_HELP;
    }
    else if (cmd == "ls" || cmd == "dir")
    {
        return COMMAND_LS;
    }
    else if (cmd == "cd")
    {
        return COMMAND_CD;
    }
    else if (cmd == "cat" || cmd == "type")
    {
        return COMMAND_CAT;
    }
    else if (cmd == "uname" || cmd == "ver")
    {
        return COMMAND_UNAME;
    }
    else if (cmd == "echo")
    {
        return COMMAND_ECHO;
    }
    else if (cmd == "clear" || cmd == "cls")
    {
        return COMMAND_CLEAR;
    }
    else if (cmd == "ps")
    {
        return COMMAND_PS;
    }
    else if (cmd == "mem")
    {
        return COMMAND_MEM;
    }
    else if (cmd == "kill")
    {
        return COMMAND_KILL;
    }
    else if (cmd == "exec")
    {
        return COMMAND_EXEC;
    }
    else if (cmd == "shutdown")
    {
        return COMMAND_SHUTDOWN;
    }
    else if (cmd == "halt")
    {
        return COMMAND_HALT;
    }
    else if (cmd == "reboot")
    {
        return COMMAND_REBOOT;
    }

    return COMMAND_NONE;
}

bool Shell::internalCommandExecute(int command, _A<CString> args)
{
    switch (command)
    {
    case COMMAND_HELP:
        formatWrite("* Mona Shell Internal Commands\n");
        formatWrite("HELP/?, LS/DIR, CD, CAT/TYPE, CHSH, UNAME/VER, ECHO, CLEAR/CLS, PS, KILL, EXEC, FD0:, CD0:\n");
        break;

    case COMMAND_CD:
        {
            CString dir;

            if (args.get_Length() < 2)
            {
                dir = startDirectory_;
            }
            else
            {
                dir = mergeDirectory(currentDirectory_, args[1]);
            }

            if (!changeDirecotory(dir))
            {
                formatWrite("%s: directory not found: %s\n", SVR, (const char*)dir);
            }

            break;
        }

    case COMMAND_LS:
        {
            if (args.get_Length() < 2)
            {
                printFiles(currentDirectory_);
            }
            else
            {
                for (int i = 1; i < args.get_Length(); i++)
                {
                    if (i > 1) formatWrite("\n");
                    CString dir = mergeDirectory(currentDirectory_, args[i]);
                    formatWrite("%s:\n", (const char*)dir);
                    printFiles(dir);
                }
            }
            break;
        }

    case COMMAND_CAT:
        {
            if (args.get_Length() < 2)
            {
                formatWrite("usage: CAT/TYPE file\n");
                break;
            }

            monapi_cmemoryinfo* mi = monapi_file_read_all(args[1]);
            if (mi == NULL) break;

            if (mi->Size > 0)
            {
                uint8_t* p = mi->Data;
                bool cr = false;
                for (uint32_t i = 0; i < mi->Size; i++)
                {
                    uint8_t b = mi->Data[i];
                    switch (b)
                    {
                        case '\r':
                            *p++ = '\n';
                            cr = true;
                            break;
                        case '\n':
                            if (!cr) *p++ = '\n';
                            cr = false;
                            break;
                        default:
                            *p++ = b;
                            cr = false;
                            break;
                    }
                }
                *p = 0;
                formatWrite((const char*)mi->Data);
            }
            monapi_cmemoryinfo_dispose(mi);
            monapi_cmemoryinfo_delete(mi);
            break;
        }

    case COMMAND_UNAME:
        {
             char ver[128];
             syscall_get_kernel_version(ver, 128);
             ver[127] = '\0';
             formatWrite("%s\n", ver);
            break;
        }

    case COMMAND_ECHO:
        {
            int len = 0;
            for (int i = 1; i < args.get_Length(); i++)
            {
                if (i > 1) len++;
                len += args[i].getLength();
            }
            char* buf = new char[len + 1], * p = buf;
            for (int i = 1; i < args.get_Length(); i++)
            {
                if (i > 1) *p++ = ' ';
                sprintf(p, args[i]);
                p += args[i].getLength();
            }
            *p = '\0';
            formatWrite("%s\n", buf);
            delete [] buf;
            break;
        }

    case COMMAND_CLEAR:
        if (doExec_)
        {
            formatWrite("can not clear while exec\n");
            break;
        }
        else
        {
            terminal_->clearScreen();
            terminal_->moveCursor(0, 0);
            terminal_->flush();
        }
        return false;

    case COMMAND_PS:
        {
            syscall_set_ps_dump();
            PsInfo info;

            formatWrite("[tid] [state]  [eip]    [esp]    [cr3]    [name]\n");

            char buf[256];
            while (syscall_read_ps_dump(&info) == 0)
            {
                sprintf(buf, "%5d %s %08x %08x %08x %s\n",
                    info.tid, info.state ? "running" : "waiting",
                    info.eip, info.esp, info.cr3, info.name);
                formatWrite(buf);
            }

            break;
        }

    case COMMAND_MEM:
        {
            MemoryInfo meminfo;
            syscall_get_memory_info(&meminfo);
            formatWrite("   Total Memory : %duint8_t\n", meminfo.totalMemoryL);
            formatWrite(" Free Page Pool : %duint8_t\n", meminfo.freePageNum * meminfo.pageSize);
            formatWrite("Total Page Pool : %duint8_t\n", meminfo.totalPageNum * meminfo.pageSize);
            break;
        }

    case COMMAND_KILL:
        {
            if (args.get_Length() < 2)
            {
                formatWrite("usage: KILL tid\n");
                break;
            }

            if (syscall_kill_thread(atoi(args[1])))
            {
                formatWrite("kill failed. Thread not found\n");
            }
            else
            {
                formatWrite("thread %d killed\n", atoi(args[1]));
            }

            break;
        }

    case COMMAND_EXEC:
        if (args.get_Length() < 2)
        {
            formatWrite("usage: EXEC command [arguments ...]\n");
        }
        else if (doExec_)
        {
            formatWrite("can not exec while exec\n");
        }
        else
        {
            _A<CString> args2(args.get_Length() - 1);
            for (int i = 1; i < args.get_Length(); i++) args2[i - 1] = args[i];
            if (commandExecute(args2, inStream_->handle(), outStream_->handle())) doExec_ = true;
        }
        break;

    case COMMAND_SHUTDOWN:
    {
        uint32_t result;

        if( args.get_Length() < 2 )
        {
                formatWrite("usage: shutdown [-rh]");
                break;
        }

        if( args[1] == "-h" || args[1] == "-H" )
        {
            result = syscall_shutdown(SHUTDOWN_HALT, SHUTDOWN_DEVICE_ALL);
        }
        else if( args[1] == "-r" || args[1] == "-R" )
        {
            result = syscall_shutdown(SHUTDOWN_REBOOT, SHUTDOWN_DEVICE_ALL);
        }

        formatWrite("result = %x\n");
        break;
    }

    case COMMAND_HALT:
    {
        syscall_shutdown(SHUTDOWN_HALT, SHUTDOWN_DEVICE_ALL);
        break;
    }

    case COMMAND_REBOOT:
    {
        syscall_shutdown(SHUTDOWN_REBOOT, SHUTDOWN_DEVICE_ALL);
        break;
    }

    default:
        break;
    }

    return true;
}

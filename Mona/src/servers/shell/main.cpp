#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Keys.h>

#include "Shell.h"

#define PROMPT "Mona>"

using namespace MonAPI;

static bool hasExited = false;
static bool callAutoExec = true;

int MonaMain(List<char*>* pekoe)
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1)) exit(1);

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == THREAD_UNKNOWN)
    {
        targetID = Message::lookupMainThread("1LINESH.SVR");
        if (targetID == THREAD_UNKNOWN)
        {
            printf("Shell Server:INIT not found\n");
            exit(1);
        }
        callAutoExec = false;
    }

    /* send */
    if (Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("Shell Server:INIT error\n");
    }

    /* service loop */
    Shell shell;
    MessageInfo info;
    while (!hasExited)
    {
        if (!Message::receive(&info) && info.arg2 & KEY_MODIFIER_DOWN)
        {
            shell.onKeyDown(info.arg1, info.arg2);
        }
    }

    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    return 0;
}

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell() : position_(0)
{
    if (!callAutoExec) printf("\n");
    printf(PROMPT);
    if (!callAutoExec) return;

    monapi_cmemoryinfo* mi = monapi_call_file_read_data("/AUTOEXEC.MSH", 0);
    if (mi == NULL) return;

    for (dword pos = 0; pos <= mi->Size; pos++)
    {
        char ch = pos < mi->Size ? (char)mi->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n')
        {
            if (position_ > 0)
            {
                commandTerminate();
                commandExecute();
            }
        }
        else
        {
            commandChar(ch);
        }
    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

Shell::~Shell()
{
}

void Shell::commandChar(char c)
{
    printf("%c", c);
    commandLine_[position_] = c;
    position_++;
}

enum
{
    COMMAND_NONE,
    COMMAND_LS,
    COMMAND_CD,
    COMMAND_CAT,
    COMMAND_CHSH,
    COMMAND_UNAME,
    COMMAND_HELP
};

int Shell::isInternalCommand(const CString& command)
{
    CString cmd = command.toLower();
    if (cmd == "ls" || cmd == "dir")
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
    else if (cmd == "chsh")
    {
        return COMMAND_CHSH;
    }
    else if (cmd == "uname" || cmd == "ver")
    {
        return COMMAND_UNAME;
    }
    else if (cmd == "help" || cmd == "?")
    {
        return COMMAND_HELP;
    }
    return COMMAND_NONE;
}

void Shell::commandExecute()
{
    printf("\n");
    _A<CString> args = this->parseCommandLine();

    if (args.get_Length() == 0)
    {
        /* command is empty */
        printf(PROMPT);
        position_ = 0;
        return;
    }

    putHistory(commandLine_);

    /* internal command */
    int isInternal;
    if ((isInternal = isInternalCommand(args[0])))
    {
        internalCommandExecute(isInternal, args);
        if (!hasExited) printf("\n%s", PROMPT);
        position_ = 0;
        return;
    }

    CString cmdLine;
    CString command = args[0].toUpper();
    if (command[0] == '/')
    {
        cmdLine = command;
    }
    else if (command.endsWith(".ELF") || command.endsWith(".EL2"))
    {
        cmdLine = "/APPS/" + command;
    }
    else if (command.endsWith(".APP"))
    {
        CString name = command.substring(0, command.getLength() - 4);
        cmdLine = "/APPS/" + name + ".APP/" + name + ".EL2";
    }
    else
    {
        cmdLine = "/APPS/" + command + ".ELF";
    }

    for (int i = 1; i < args.get_Length(); i++)
    {
        cmdLine += ' ';
        cmdLine += args[i];
    }

    monapi_call_elf_execute_file(cmdLine, 1);

    printf("\n%s", PROMPT);
    position_ = 0;
}

void Shell::internalCommandExecute(int command, _A<CString> args)
{
    switch (command)
    {
    case COMMAND_CD:
        {
            if (args.get_Length() < 2)
            {
                printf("usage: CD directory\n");
            }
            else if (syscall_cd(args[1]))
            {
                printf("directory not found: %s\n", (const char*)args[1]);
            }
            break;
        }
    case COMMAND_LS:
        {
            char name[15];
            int  size;

            if (syscall_dir_open())
            {
                printf("dir open error\n");
                break;
            }

            while (syscall_dir_read(name, &size) == 0)
            {
                printf("%s\n", name);
            }

            syscall_dir_close();
            break;
        }
    case COMMAND_CAT:
        {
            if (args.get_Length() < 2)
            {
                printf("usage: CAT/TYPE file\n");
                break;
            }

            monapi_cmemoryinfo* mi = monapi_call_file_read_data(args[1], 1);
            if (mi == NULL) break;

            if (mi->Size > 0)
            {
                byte* p = mi->Data;
                bool cr = false;
                for (dword i = 0; i < mi->Size; i++)
                {
                    byte b = mi->Data[i];
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
                printf((const char*)mi->Data);
            }
            monapi_cmemoryinfo_dispose(mi);
            monapi_cmemoryinfo_delete(mi);
            break;
        }
    case COMMAND_CHSH:
        if (monapi_call_elf_execute_file("/SERVERS/1LINESH.SVR", 1) != 0) break;
        for (MessageInfo msg;;)
        {
            if (Message::receive(&msg) != 0) continue;
            if (msg.header == MSG_SERVER_START_OK) break;
        }
        hasExited = true;
        break;
    case COMMAND_UNAME:
        {
            char ver[128];
            syscall_get_kernel_version(ver, 128);
            ver[127] = '\0';
            printf("%s\n", ver);
            break;
        }
    case COMMAND_HELP:
        printf("* Mona Shell Internal Commands\n");
        printf("LS/DIR, CD, CAT/TYPE, CHSH, UNAME/VER, HELP/?\n");
        break;
    default:
        break;
    }
}

void Shell::commandTerminate()
{
    commandChar('\0');
}

void Shell::putHistory(const CString& command)
{
    history.add(command);
}

CString Shell::getHistory()
{
    if (history.isEmpty()) return "";
    return history.get(0);
}

void Shell::backspace() {

    if (position_ == 0) {
        /* donothing */
        return;
    }

    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - 1, y);
    printf(" ");
    syscall_set_cursor(x - 1, y);

    /* backspace */
    position_--;
}

int Shell::onKeyDown(int keycode, int modifiers) {

    switch(keycode) {
    case(Keys::A):
    case(Keys::B):
    case(Keys::C):
    case(Keys::D):
    case(Keys::E):
    case(Keys::F):
    case(Keys::G):
    case(Keys::H):
    case(Keys::I):
    case(Keys::J):
    case(Keys::K):
    case(Keys::L):
    case(Keys::M):
    case(Keys::N):
    case(Keys::O):
    case(Keys::P):
    case(Keys::Q):
    case(Keys::R):
    case(Keys::S):
    case(Keys::T):
    case(Keys::U):
    case(Keys::V):
    case(Keys::W):
    case(Keys::X):
    case(Keys::Y):
    case(Keys::Z):
    case(Keys::Decimal):
    case(Keys::D0):
    case(Keys::D1):
    case(Keys::D2):
    case(Keys::D3):
    case(Keys::D4):
    case(Keys::D5):
    case(Keys::D6):
    case(Keys::D7):
    case(Keys::D8):
    case(Keys::D9):
    case(Keys::NumPad1):
    case(Keys::NumPad2):
    case(Keys::NumPad3):
    case(Keys::NumPad4):
    case(Keys::NumPad5):
    case(Keys::NumPad6):
    case(Keys::NumPad7):
    case(Keys::NumPad8):
    case(Keys::NumPad9):
    case(Keys::NumPad0):
    case(Keys::Subtract):
    case(Keys::Add):
    case(Keys::Space):
    case(Keys::Divide):
    case(Keys::OemPeriod):
    case(Keys::OemQuestion):
        KeyInfo key;
        key.keycode = keycode;
        key.modifiers = modifiers;
        commandChar(Keys::ToChar(key));
        break;
    case(Keys::Enter):
        commandTerminate();
        commandExecute();
        break;

    case(Keys::Up):
        printf("up");
        break;
    case(Keys::Down):
        printf("down");
        break;
    case(Keys::Left):
        printf("left");
        break;
    case(Keys::Right):
        printf("right");
        break;
    case(Keys::Back):
        backspace();
        break;
    default:
        break;

    }
    return 0;
}

_A<CString> Shell::parseCommandLine()
{
    _A<CString> args = CString(this->commandLine_).split(' ');
    int size = 0;
    FOREACH (CString, arg, args)
    {
        if (arg != NULL) size++;
    }

    _A<CString> ret(size);
    int i = 0;
    FOREACH (CString, arg, args)
    {
        if (arg == NULL) continue;

        ret[i++] = arg;
    }

    return ret;
}

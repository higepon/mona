#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Keys.h>

#include "Shell.h"

#define SVR "Shell Server"
#define FONT_WIDTH 8

using namespace MonAPI;

static bool hasExited = false;
static bool callAutoExec = true;
static dword waiting = THREAD_UNKNOWN;
static CString AppsDir  = "/APPS";
static CString StartDir = "/APPS";
static Screen screen;

int MonaMain(List<char*>* pekoe)
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1)) exit(1);
    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1)) exit(1);

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == THREAD_UNKNOWN)
    {
        targetID = Message::lookupMainThread("1LINESH.SVR");
        if (targetID == THREAD_UNKNOWN)
        {
            printf("%s: INIT not found\n", SVR);
            exit(1);
        }
        callAutoExec = false;
    }

    /* send */
    if (Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("%s: INIT error\n", SVR);
    }

    /* service loop */
    Shell shell;
    MessageInfo msg;
    while (!hasExited)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_KEY_VIRTUAL_CODE:
                if (msg.arg2 & KEY_MODIFIER_DOWN)
                {
                    shell.onKeyDown(msg.arg1, msg.arg2);
                }
                break;
            case MSG_PROCESS_TERMINATED:
                if (waiting == msg.arg1)
                {
                    shell.printPrompt("\n");
                    waiting = THREAD_UNKNOWN;
                }
                break;
        }
    }

    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;
}

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell() : position_(0)
{
    this->current = StartDir;
    this->makeApplicationList();

    if (!callAutoExec)
    {
        this->printPrompt("\n");
        return;
    }

    this->executeMSH("/AUTOEXEC.MSH");
    callAutoExec = false;
    this->printPrompt("\n");
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
    COMMAND_ECHO,
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
    else if (cmd == "echo")
    {
        return COMMAND_ECHO;
    }
    else if (cmd == "help" || cmd == "?")
    {
        return COMMAND_HELP;
    }
    return COMMAND_NONE;
}

void Shell::commandExecute(bool prompt)
{
    if (prompt) printf("\n");
    _A<CString> args = this->parseCommandLine();

    if (args.get_Length() == 0)
    {
        /* command is empty */
        this->printPrompt();
        position_ = 0;
        return;
    }

    putHistory(commandLine_);

    /* internal command */
    int isInternal;
    if ((isInternal = isInternalCommand(args[0])))
    {
        internalCommandExecute(isInternal, args);
        if (!hasExited && prompt)
        {
            this->printPrompt("\n");
        }
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
        cmdLine = AppsDir + "/" + command;
    }
    else if (command.endsWith(".APP"))
    {
        CString name = command.substring(0, command.getLength() - 4);
        cmdLine = AppsDir + "/" + name + ".APP/" + name + ".EL2";
    }
    else
    {
        for (int i = 0; i < this->apps.size(); i++)
        {
            CString file = apps.get(i);
            if (file == command + ".EL2" || file == command + ".MSH")
            {
                cmdLine = AppsDir + "/" + file;
                break;
            }
            else if (file == command + ".APP")
            {
                cmdLine = AppsDir + "/" + file + "/" + command + ".EL2";
                break;
            }
        }
        if (cmdLine == NULL) cmdLine = AppsDir + "/" + command + ".ELF";
    }

    if (cmdLine.endsWith(".MSH"))
    {
        this->executeMSH(cmdLine);
        if (waiting != THREAD_UNKNOWN) this->printPrompt("\n");
        return;
    }

    for (int i = 1; i < args.get_Length(); i++)
    {
        cmdLine += ' ';
        cmdLine += args[i];
    }

    dword tid;
    int result = monapi_call_elf_execute_file_get_tid(cmdLine, 1, &tid);

    position_ = 0;
    if (!callAutoExec && result == 0)
    {
        waiting = tid;
    }
    else
    {
        this->printPrompt("\n");
    }
}

void Shell::internalCommandExecute(int command, _A<CString> args)
{
    switch (command)
    {
    case COMMAND_CD:
        {
            if (args.get_Length() < 2)
            {
                this->current = StartDir;
                break;
            }
            CString dir = this->mergeDirectory(this->current, args[1]);
            if (syscall_cd(dir) != 0)
            {
                printf("%s: directory not found: %s\n", SVR, (const char*)dir);
                break;
            }
            this->current = dir;
            break;
        }
    case COMMAND_LS:
        {
            if (args.get_Length() < 2)
            {
                printFiles(this->current);
            }
            else
            {
                for (int i = 1; i < args.get_Length(); i++)
                {
                    if (i > 1) printf("\n");
                    CString dir = this->mergeDirectory(this->current, args[i]);
                    printf("%s:\n", (const char*)dir);
                    printFiles(dir);
                }
            }
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
    case COMMAND_ECHO:
        for (int i = 1; i < args.get_Length(); i++)
        {
            if (i > 1) printf(" ");
            printf("%s", (const char*)args[i]);
        }
        printf("\n");
        break;
    case COMMAND_HELP:
        printf("* Mona Shell Internal Commands\n");
        printf("LS/DIR, CD, CAT/TYPE, CHSH, UNAME/VER, ECHO, HELP/?\n");
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

void Shell::onKeyDown(int keycode, int modifiers)
{
    if (waiting != THREAD_UNKNOWN)
    {
        this->printPrompt("\n");
        waiting = THREAD_UNKNOWN;
        if (keycode == Keys::Enter) return;
    }

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
        commandExecute(true);
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

int Shell::makeApplicationList()
{
    char name[15];
    int  size;

    if (syscall_cd(AppsDir) != 0)
    {
        printf("%s: application list error\n", SVR);
        return 1;
    }

    if (syscall_dir_open() != 0)
    {
        printf("%s: application dir open error\n", SVR);
        return 2;
    }

    while (syscall_dir_read(name, &size) == 0)
    {
        CString file = name;
        if (file.endsWith(".EL2") || file.endsWith(".APP") || file.endsWith(".MSH"))
        {
            apps.add(name);
        }
    }

    syscall_dir_close();

    if (AppsDir != this->current)
    {
        if (syscall_cd(this->current) != 0)
        {
            this->current = "/";
            syscall_cd(this->current);
        }
    }

    return 0;
}

void Shell::printPrompt(const CString& prefix /*= NULL*/)
{
    if (prefix != NULL) printf("%s", (const char*)prefix);
    printf("[Mona]%s> ", (const char*)this->current);
}

CString Shell::getParentDirectory(const CString& dir)
{
    if (dir == NULL || dir == "/") return "/";

    int p = dir.lastIndexOf('/');
    if (p < 1) return "/";

    return dir.substring(0, p);
}

CString Shell::mergeDirectory(const CString& dir1, const CString& dir2)
{
    if (dir2.startsWith("/")) return dir2.toUpper();

    CString ret = dir1;
    _A<CString> dirs = dir2.split('/');
    FOREACH (CString, d, dirs)
    {
        if (d == NULL || d == ".") continue;

        if (d == "..")
        {
            ret = this->getParentDirectory(ret);
        }
        else
        {
            if (ret != "/") ret += '/';
            ret += d.toUpper();
        }
    }
    return ret;
}

void Shell::printFiles(const CString& dir)
{
    char name[15];
    int  size;

    if (syscall_cd(dir) != 0)
    {
        printf("%s: directory not found: %s\n", SVR, (const char*)dir);
        return;
    }
    if (syscall_dir_open())
    {
        printf("%s: can not open directory: %s\n", SVR, (const char*)dir);
        return;
    }

    CString spc = "               ";
    int w = 0, sw = screen.getWidth();
    while (syscall_dir_read(name, &size) == 0)
    {
        CString file = name;
        if (file == "." || file == "..") continue;

        file = (file + spc).substring(0, 15);
        int fw = FONT_WIDTH * file.getLength();
        if (w + fw >= sw)
        {
            printf("\n");
            w = 0;
        }
        printf("%s", (const char*)file);
        w += fw;
    }
    printf("\n");

    syscall_dir_close();
}

void Shell::executeMSH(const CString& msh)
{
    monapi_cmemoryinfo* mi = monapi_call_file_read_data(msh, 1);
    if (mi == NULL) return;

    for (dword pos = 0, start = 0; pos <= mi->Size; pos++)
    {
        char ch = pos < mi->Size ? (char)mi->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n')
        {
            int len = pos - start;
            bool prompt = true;
            if (len > 0)
            {
                if (mi->Data[start] == '@')
                {
                    start++;
                    len--;
                    prompt = false;
                }
            }
            if (len > 0)
            {
                if (len > 127) len = 127;
                strncpy(this->commandLine_, (const char*)&mi->Data[start], len);
                this->position_ = len;
                this->commandLine_[len] = '\0';
                if (prompt)
                {
                    this->printPrompt("\n");
                    printf("%s", this->commandLine_);
                }
                this->commandExecute(prompt);
            }
            start = pos + 1;
        }
    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

#include <monapi.h>
#include <ShellServer.h>
#include <Shell.h>
#include <monapi/Keys.h>

static bool isExited = false;
static bool callAutoExec = true;

using namespace MonAPI;

/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
int MonaMain(List<char*>* pekoe)
{
    Server* server = new ShellServer();
    server->service();
    return 0;
}

/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
char* const Shell::PROMPT = "Mona>";
ShellServer::ShellServer()
{
}

ShellServer::~ShellServer()
{
}

void ShellServer::service()
{
    /* look up */
    dword myID = System::getThreadID();

    dword keysvrID = Message::lookupMainThread("KEYBDMNG.SVR");
    if (keysvrID == 0xFFFFFFFF)
    {
        printf("Shell:KeyBoardServer not found\n");
        exit(1);
    }

    /* send */
    if (Message::send(keysvrID, MSG_KEY_REGIST_TO_SERVER, myID))
    {
        printf("Shell: key regist error\n");
    }

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == 0xFFFFFFFF)
    {
        targetID = Message::lookupMainThread("1LINESH.SVR");
        if (targetID == 0xFFFFFFFF)
        {
            printf("ShellServer:INIT not found\n");
            exit(1);
        }
        callAutoExec = false;
    }

    /* send */
    if (Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("ShellServer:INIT error\n");
    }

    /* service loop */
    Shell shell;
    MessageInfo info;
    while (!isExited)
    {
        if (!Message::receive(&info) && info.arg2 & KEY_MODIFIER_DOWN)
        {
            shell.onKeyDown(info.arg1, info.arg2);
        }
    }

    /* send */
    if (Message::send(keysvrID, MSG_KEY_UNREGIST_FROM_SERVER, myID))
    {
        printf("Shell: key unregist error\n");
    }
}

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell() : position_(0)
{
    history_ = new HList<char*>();
    if (!callAutoExec) printf("\n");
    printf("%s", PROMPT);
    if (!callAutoExec) return;

    FileInputStream fis("/AUTOEXEC.MSH");
    if (fis.open() != 0) return;

    int len = fis.getFileSize();
    byte* data = new byte[len];
    fis.read(data, len);
    fis.close();

    for (int pos = 0; pos <= len; pos++)
    {
        char ch = pos < len ? (char)data[pos] : '\n';
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

    delete [] data;
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

int Shell::isInternalCommand(const char* command)
{
    int len = strlen(command);
    if (len > 15) len = 15;
    char cmd[16];
    for (int i = 0; i < len; i++)
    {
        char ch = command[i];
        if ('a' <= ch && ch <= 'z') ch -= 'a' - 'A';
        cmd[i] = ch;
    }
    cmd[len] = '\0';

    if (strcmp(cmd, "LS") == 0 || strcmp(cmd, "DIR") == 0)
    {
        return COMMAND_LS;
    }
    else if (strcmp(cmd, "CD") == 0)
    {
        return COMMAND_CD;
    }
    else if (strcmp(cmd, "CAT") == 0 || strcmp(cmd, "TYPE") == 0)
    {
        return COMMAND_CAT;
    }
    else if (strcmp(cmd, "CHSH") == 0)
    {
        return COMMAND_CHSH;
    }
    else if (strcmp(cmd, "UNAME") == 0 || strcmp(cmd, "VER") == 0)
    {
        return COMMAND_UNAME;
    }
    else if (strcmp(cmd, "HELP") == 0 || strcmp(cmd, "?") == 0)
    {
        return COMMAND_HELP;
    }
    return COMMAND_NONE;
}

void Shell::commandExecute()
{
    printf("\n");

    if (position_ < 2)
    {
        /* command is empty */
        printf("%s", PROMPT);
        position_ = 0;
        return;
    }

    putHistory(commandLine_);

    /* list initilize */
    CommandOption list;
    list.next = NULL;

    char* command = strtok(commandLine_, " ");
    char* arg;
    CommandOption* option = NULL;

    while ((arg = strtok(NULL, " ")) != NULL)
    {
        option = new CommandOption;
        strncpy(option->str, arg, 32);
        option->next = list.next;
        list.next = option;
    }

    /* internal command */
    int isInternal;
    if ((isInternal = isInternalCommand(command)))
    {
        internalCommandExecute(isInternal, option);
        for (option = list.next; option; option = option->next)
        {
            delete option;
        }
        if (!isExited) printf("\n%s", PROMPT);
        position_ = 0;
        return;
    }

    char path[128], * bundle;

    if (command[0] == '/')
    {
        strncpy(path, command, sizeof(path));
        char* p = &command[strlen(command)];
        for (; *p != '/' && command < p; p--);
        p++;
        for (int i = 0;; i++, p++)
        {
            command[i] = *p;
            if (*p == '\0') break;
        }
    }
    else if (strstr(command, ".ELF") != NULL)
    {
        sprintf(path, "/APPS/%s", command);
    }
    else if ((bundle = strstr(command, ".APP")) != NULL)
    {
        *bundle = '\0';
        sprintf(path, "/APPS/%s.APP/%s.ELF", command, command);
        strcpy(bundle, ".ELF");
    }
    else
    {
        sprintf(path, "/APPS/%s.ELF", command);
    }

    executeProcess(path, command, &list);

    for (option = list.next; option; option = option->next)
    {
        delete option;
    }

    printf("\n%s", PROMPT);
    position_ = 0;
}

void Shell::internalCommandExecute(int command, CommandOption* option)
{
    switch (command)
    {
    case COMMAND_CD:
        {
            if (option == NULL)
            {
                printf("usage: CD directory\n");
            }
            else if (syscall_cd(option->str))
            {
                printf("directory not found: %s\n", option->str);
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
            if (option == NULL)
            {
                printf("usage: CAT/TYPE file\n");
                break;
            }

            FileInputStream fis(option->str);
            if (fis.open() != 0)
            {
                printf("file open error: %s\n", option->str);
                break;
            }
            int size = fis.getFileSize();
            byte* buf = new byte[size + 1];
            fis.read(buf, size);
            fis.close();
            if (size > 0)
            {
                byte* p = buf;
                bool cr = false;
                for (int i = 0; i < size; i++)
                {
                    byte b = buf[i];
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
                printf((const char*)buf);
            }
            delete [] buf;
            break;
        }
    case COMMAND_CHSH:
        if (executeProcess("/SERVERS/1LINESH.SVR", "1LINESH.SVR", NULL) != 0) break;
        for (MessageInfo msg;;)
        {
            if (Message::receive(&msg) != 0) continue;
            if (msg.header == MSG_SERVER_START_OK) break;
        }
        isExited = true;
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

void Shell::commandTerminate() {
    commandChar('\0');
}

void Shell::putHistory(const char* command) {

    char* str = new char[strlen(command) + 1];
    strcpy(str, command);

    history_->add(str);
}

char* Shell::getHistory() {

    if (history_->isEmpty()) {
        return "";
    }
    return history_->get(0);
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

int Shell::executeProcess(const char* path, const char* name ,CommandOption* option)
{
    int result = syscall_load_process(path, name, option);

    switch(result)
    {

      case(0):
          break;
      case(1):
          printf("File not found");
          break;
      case(2):
          printf("load Process memory allocate error");
          break;
      case(3):
          printf("File read error");
          break;
      case(4):
          printf("Shared Memory error1");
          break;
      case(5):
          printf("Shared Memory error2");
          break;
      default:
          break;
    }

    return result;
}

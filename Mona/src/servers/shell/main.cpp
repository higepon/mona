#include <monapi.h>
#include <KeyBoardManager.h>
#include <ShellServer.h>
#include <Shell.h>

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

    dword targetID = Message::lookupMainThread("KEYBDMNG.SVR");
    if (targetID == 0xFFFFFFFF)
    {
        printf("Shell:KeyBoardServer not found\n");
        exit(1);
    }

    /* create message for KEYBDMNG.SVR */
    MessageInfo info;
    Message::create(&info, MSG_KEY_REGIST_TO_SERVER, myID, 0, 0, NULL);

    /* send */
    if (Message::send(targetID, &info))
    {
        printf("Shell: key regist error\n");
    }

    /* Server start ok */
    targetID = Message::lookupMainThread("INIT");
    if (targetID == 0xFFFFFFFF)
    {
        printf("ShellServer:INIT not found\n");
        exit(1);
    }

    /* create message */
    Message::create(&info, MSG_SERVER_START_OK, 0, 0, 0, NULL);

    /* send */
    if (Message::send(targetID, &info))
    {
        printf("ShellServer:INIT error\n");
    }

    /* service loop */
    Shell shell;
    for (;;)
    {
        if (!Message::receive(&info) && info.arg2 & KEY_MODIFIER_DOWN)
        {
            shell.onKeyDown(info.arg1, info.arg2);
        }
    }
}

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell() : position_(0)
{
    history_ = new HList<char*>();
    printf("%s", PROMPT);

    FileInputStream* fis = new FileInputStream("/AUTOEXEC.MSH");
    if (fis->open() == 0)
    {
        int len = fis->getFileSize();
        byte* data = new byte[len];
        fis->read(data, len);
        fis->close();
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

    delete fis;
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

#define COMMAND_LS 1
#define COMMAND_CD 2

int Shell::isInternalCommand(char* command)
{
    if (!strcmp(command, "LS") || !strcmp(command, "DIR"))
    {
        return COMMAND_LS;
    }
    else if (!strcmp(command, "CD"))
    {
        return COMMAND_CD;
    }
    else
    {
        return 0;
    }
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
    CommandOption* option;

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
        printf("\n%s", PROMPT);
        position_ = 0;
        return;
    }

    char path[128];

    if (strstr(command, ".ELF"))
    {
        sprintf(path, "/APPS/%s", command);
    }
    else
    {
        sprintf(path, "/APPS/%s.ELF", command);
    }

    int result = syscall_load_process(path, command, &list);

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

    for (option = list.next; option; option = option->next)
    {
        delete option;
    }

    printf("\n%s", PROMPT);
    position_ = 0;
}

void Shell::internalCommandExecute(int command, CommandOption* option)
{
    switch(command)
    {
    case(COMMAND_CD):
        {
            char* path = option ? option->str : NULL;
            if (path == NULL)
            {
                break;
            }

            if (syscall_cd(path))
            {
                printf("directory %s not found\n", path);
                break;
            }
            break;
        }
    case(COMMAND_LS):
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
    default:
        break;
    }
}

void Shell::commandTerminate() {
    commandChar('\0');
}

void Shell::putHistory(char* command) {

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
    case(Keys::Subtract):
    case(Keys::Add):
    case(Keys::Space):
    case(Keys::Divide):

        commandChar(KeyBoardManager::toChar(keycode));
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

    case(Keys::NumPad0):

        strcpy(commandLine_, getHistory());
        printf("%s", commandLine_);
        break;
    default:
        break;

    }
    return 0;
}

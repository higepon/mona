#include <userlib.h>
#include <KeyBoardManager.h>
#include <Shell.h>

/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
int MonaMain(List<char*>* pekoe) {

    Server* server = new ShellServer();
    server->service();

    return 0;
}

/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
char* const Shell::PROMPT = "Mona>";
ShellServer::ShellServer() {
}

ShellServer::~ShellServer() {
}

void ShellServer::service() {

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
    if (Message::send(targetID, &info)) {
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
    if (Message::send(targetID, &info)) {
        printf("ShellServer:INIT error\n");
    }

    /* service loop */
    Shell shell;
    for (;;) {
        if (!Message::receive(&info) && info.arg2 & KEY_MODIFIER_DOWN) {
            shell.onKeyDown(info.arg1, info.arg2);
        }
    }
}

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell() : position_(0) {

    history_ = new HList<char*>();
    printf("%s", PROMPT);

    FileInputStream* fis = new FileInputStream("/AUTOEXEC.MSH");
    if (fis->open() == 0) {
        int len = fis->getFileSize();
        byte* data = new byte[len];
        fis->read(data, len);
        fis->close();
        for (int pos = 0; pos <= len; pos++) {
            char ch = pos < len ? (char)data[pos] : '\n';
            if (ch == '\r' || ch == '\n') {
                if (position_ > 0) {
                    commandTerminate();
                    commandExecute();
                }
            } else {
                commandChar(ch);
            }
        }
        delete [] data;
    }
    delete fis;
}

Shell::~Shell() {
}

void Shell::commandChar(char c) {

    printf("%c", c);
    commandLine_[position_] = c;
    position_++;
}

void Shell::commandExecute() {

    printf("\n");

    if (position_ < 2) {
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

    while ((arg = strtok(NULL, " ")) != NULL) {

        option = new CommandOption;
        strncpy(option->str, arg, 32);
        option->next = list.next;
        list.next = option;
    }

    char path[128];

    if (strstr(command, ".ELF"))
    {
        sprintf(path, "/APP/%s", command);
    }
    else
    {
        sprintf(path, "/APP/%s.ELF", command);
    }

    int result = syscall_load_process(path, command, &list);

    switch(result) {

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

    for (option = list.next; option; option = option->next) {
        delete option;
    }

    printf("\n%s", PROMPT);
    position_ = 0;
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
    case(VK_A):
    case(VK_B):
    case(VK_C):
    case(VK_D):
    case(VK_E):
    case(VK_F):
    case(VK_G):
    case(VK_H):
    case(VK_I):
    case(VK_J):
    case(VK_K):
    case(VK_L):
    case(VK_M):
    case(VK_N):
    case(VK_O):
    case(VK_P):
    case(VK_Q):
    case(VK_R):
    case(VK_S):
    case(VK_T):
    case(VK_U):
    case(VK_V):
    case(VK_W):
    case(VK_X):
    case(VK_Y):
    case(VK_Z):
    case(VK_PERIOD):
    case(VK_0):
    case(VK_1):
    case(VK_2):
    case(VK_3):
    case(VK_4):
    case(VK_5):
    case(VK_6):
    case(VK_7):
    case(VK_8):
    case(VK_9):
    case(VK_TEN_1):
    case(VK_TEN_2):
    case(VK_TEN_3):
    case(VK_TEN_4):
    case(VK_TEN_5):
    case(VK_TEN_6):
    case(VK_TEN_7):
    case(VK_TEN_8):
    case(VK_TEN_9):
    case(VK_TEN_MINUS):
    case(VK_TEN_PLUS):
    case(VK_TEN_PERIOD):
    case(VK_SPACE):
    case(VK_SLASH):

        commandChar(KeyBoardManager::toChar(keycode));
        break;
    case(VK_ENTER):
        commandTerminate();
        commandExecute();
        break;
    case(VK_BACKSPACE):
        backspace();
        break;

    case(VK_TEN_0):

        strcpy(commandLine_, getHistory());
        printf("%s", commandLine_);
        break;
    default:
        break;

    }
    return 0;
}

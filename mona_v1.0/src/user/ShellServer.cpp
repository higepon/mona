#include <userlib.h>
#include <KeyBoardManager.h>
#include <Shell.h>

/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
int main() {

    Server* server = new ShellServer();
    server->service();

    return 0;
}

/*----------------------------------------------------------------------
    ShellServer
----------------------------------------------------------------------*/
ShellServer::ShellServer() {
}

ShellServer::~ShellServer() {
}

void ShellServer::service() {

    /* look up */
    dword myPid   = Message::lookup("SHELL.SVR");
    dword destPid = Message::lookup("KEYBDMNG.SVR");
    if (destPid == 0) {
        printf("process KEYBDMNG.SVR not found\n");
        for (;;);
    }

    /* create message for KEYBDMNG.SVR */
    MessageInfo info;
    info.header = MSG_KEY_REGIST_TO_SERVER;
    info.arg1   = myPid;

    /* send */
    if (Message::send(destPid, &info)) {
        printf("regist error\n");
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

    printf("Mona>");
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
    syscall_load_process(commandLine_);
    printf("\nMona>");
    position_ = 0;
}

void Shell::commandTerminate() {
    commandChar('\0');
}

void Shell::backspace() {

    if (position_ == 0) {
        /* donothing */
        return;
    }

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
    case(VK_TEN_0):
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

        commandChar(KeyBoardManager::toChar(keycode));
        break;
    case(VK_ENTER):
        commandTerminate();
        commandExecute();
        break;
    case(VK_BACKSPACE):
        backspace();
        break;
    default:
        break;

    }
    return 0;
}

#include <userlib.h>
#include <KeyBoardManager.h>

int listener();
int disp();
static char buf[32];
static bool come;
static Mutex* mutex;

int main() {

    dword id;
    mutex = new Mutex();

    MessageInfo info;
    char* dest = "USER.ELF";
    info.header = MSG_KEY_REGIST_TO_SERVER;
    info.arg1   = (dword)dest;

    Message::send("KEYBDMNG.SVR", &info);

    if (mutex->init()) {
        print("mutex init errror\n");
        exit(-1);
    }

    if (!(id = mthread_create((dword)listener))) {
        print("mthread create error\n");
        exit(-1);
    }

    if (mthread_join(id)) {
        print("mthread join error\n");
        exit(-1);
    }

    disp();
    return 0;
}

int disp() {

    for (;;) {
        if (come) {
            mutex->lock();
            print(buf);
            buf[0] = '0';
            come = false;
            mutex->unlock();
        }
    }
}

int listener() {

    MessageInfo message;

    for (;;) {
        if (!Message::receive(&message)) {
            mutex->lock();
            buf[0] = '<';
            buf[1] = (char)message.arg1;
            buf[2] = '>';
            buf[3] = '\0';
            come = true;
            mutex->unlock();
        }
    }
}

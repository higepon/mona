#include <userlib.h>

int listener();
int disp();
static char buf[32];
static bool come;

int main() {

    dword id;

    Mutex* mutex = new Mutex();

    if (!(id = mthread_create((dword)listener))) {
        print("mthread create error\n");
        exit(-1);
    }

    if (mthread_join(id)) {
        print("mthread join error\n");
        exit(-1);
    }

    disp();
}

int disp() {

    for (;;) {
        if (come) {
            print(buf);
            buf[0] = '0';
            come = false;
        }
    }

}

int listener() {

    Message message;

    for (;;) {
        if (!_receive(&message)) {
            buf[0] = '<';
            buf[1] = (char)message.arg1;
            buf[2] = '>';
            buf[3] = '\0';
            come = true;
        }
    }
}

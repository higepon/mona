#include <userlib.h>

int listener();
int disp();
static char buf[32];
static bool come;
static Mutex* mutex;

int main() {

    for (;;);

    dword id;
    mutex = new Mutex();

    print("hello1");

    if (mutex->init()) {
        print("mutex init errror\n");
        exit(-1);
    }

    print("hello2");

    if (!(id = mthread_create((dword)listener))) {
        print("mthread create error\n");
        exit(-1);
    }

    print("hello3");

    if (mthread_join(id)) {
        print("mthread join error\n");
        exit(-1);
    }

    print("hello4");

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

    Message message;

    for (;;) {
        if (!_receive(&message)) {
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

#include <userlib.h>
#include <string.h>

class myApplication : public MonaApplication {

  public:
    myApplication();
    virtual ~myApplication() {}

  public:
    virtual int main(List<char*>* pekoe);
    virtual void onKeyDown(int keycode, int modifiers);
};

myApplication::myApplication() : MonaApplication() {
}

int myApplication::main(List<char*>* pekoe) {

    Date dt;
    Screen screen;
    int x, y;

    for (;;) {
        syscall_get_cursor(&x, &y);
        syscall_set_cursor(0, 0);
        dt.refresh();
        printf("%d/%d/%d %d:%d:%d   ", dt.year(), dt.month(), dt.day(), dt.hour(), dt.min(), dt.sec());
        syscall_set_cursor(x, y);
    }
    return 0;
}

void myApplication::onKeyDown(int keycode, int modifiers) {
}

int MonaMain(List<char*>* pekoe) {

    monaApp = new myApplication();
    monaApp->main(pekoe);
    return 0;
}

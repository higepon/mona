#include <userlib.h>

class myApplication : public MonaApplication {

  public:
    myApplication(char* name);
    virtual ~myApplication() {}

  public:
    virtual int main(List<char*>* pekoe);
    virtual void onKeyDown(int keycode, int modifiers);
};

myApplication::myApplication(char* name) : MonaApplication(name) {
}

int myApplication::main(List<char*>* pekoe) {

    /* arguments */
    printf("\nargument list\n");
    for (int i = 0; i < pekoe->size(); i++) {
        printf("[%s]\n", pekoe->get(i));
    }

    Date dt;
    printf("%d/%d/%d %d:%d:%d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.min(), dt.sec());

//    char str[128];
//    sprintf(str, "test sprintf = %x", 0x12345678);
//    print(str);

    /* draw rectangle */
    Screen screen;
    //    screen.fillRect16(10, 10, 40, 50, Color::rgb(105, 141, 148));
    screen.fillRect16(10, 10, 40, 50, Color::rgb(0x89, 0x67, 0x81));
    screen.circle16(50, 50, 20, Color::rgb(0x89, 0x67, 0x81));
    screen.fillCircle16(100, 100, 20, Color::rgb(0x10, 0x67, 0x81));

    Screen* virtualScreen = new VirtualScreen();
    virtualScreen->fillRect16(50, 50, 40, 40, Color::rgb(0x12, 0x34, 0x56));
    printf("%s", Screen::bitblt(&screen, 0, 0, 40, 40, virtualScreen, 50, 50) ? "OK" : "NG");

    return 0;
}

void myApplication::onKeyDown(int keycode, int modifiers)
{
}

int MonaMain(List<char*>* pekoe)
{
    monaApp = new myApplication("HELLO.ELF");
    return monaApp->main(pekoe);
}

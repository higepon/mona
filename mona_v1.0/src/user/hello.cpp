#include <userlib.h>
#include <string.h>
#include <collection.h>

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

    /* draw rectangle */
    Screen screen;
    screen.fillRect16(10, 10, 40, 50, Color::rgb(105, 141, 148));

    /* memory share between this process and SHELL.SVR */
    dword pid = Message::lookup("SHELL.SVR");
    MemoryMap* mm = MemoryMap::create();
    dword sharedId = mm->map(pid, 0x90000000, 0x90005000, 4096);
    strcpy((char*)0x90005000, "data share Mona");
    mm->unmap(sharedId);

    /* unmap? */
    //printf("%s", (char*)0x90005000);

    /* floppy read/write test */
    char buf[1024];
    memset(buf      , (byte)0x12, 512);
    memset(buf + 512, (byte)0x34, 512);

    StorageDevice* device = new Floppy(Floppy::FLOPPY_1);

    printf("device block size=%d\n", device->getBlockSize());

    device->open();
    device->write(5, (byte*)buf, 2);

    memset(buf, (byte)0x00, 1024);

    device->read(5, (byte*)buf, 2);
    device->close();
    delete device;

    for (int i = 0; i < 10; i++) {
        printf("[%x]", buf[i]);
    }
    printf("\n--------------------------\n");

    for (int i = 0; i < 10; i++) {
        printf("[%x]", buf[512 + i]);
    }

    Screen* virtualScreen = new VirtualScreen();
    virtualScreen->fillRect16(50, 50, 40, 40, Color::rgb(0x12, 0x34, 0x56));
    printf("%s", Screen::bitblt(&screen, 0, 0, 40, 40, virtualScreen, 50, 50) ? "OK" : "NG");

    return 0;
}

void myApplication::onKeyDown(int keycode, int modifiers) {
    printf("down\n");
}

int MonaMain(List<char*>* pekoe) {

    monaApp = new myApplication("HELLO.ELF");
    monaApp->main(pekoe);

//     byte buf[512];
//     memset(buf, 0, 512);

//     InputStream* stream = new FileInputStream("HELLO.ELF");
//     stream->open();
//     stream->read(buf, 510);
//     stream->close();


//     for (int i = 0; i < 50; i++) {
//         printf("[%x]", (int)(buf[i]));
//     }

//     dword mypid = Message::lookup("HELLO.ELF");
//     dword pid   = Message::lookup("MAP.SVR");

//     MessageInfo message;
//     message.header = MSG_MAP;
//     message.arg1   = mypid;
//     message.arg2   = 0x20000000;
//     message.from   = mypid;
//     strcpy(message.str, "mona://device/fd/buffer");

//     outportb(0x3f2, 0x1c);

//     if (Message::send(pid, &message)) {
//         printf("send error\n");
//     }

//     for (;;) {
//         /* receive */
//         if (!Message::receive(&message)) {

//             switch(message.header) {

//             case MSG_RESULT_OK:
//                 printf("contents=%s", (char*)0x20000000);
//                 break;

//             default:
//                 /* igonore this message */
//                 break;
//             }
//         }
//     }

    return 0;
}

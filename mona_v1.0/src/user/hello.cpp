#include <userlib.h>
#include <string.h>
#include <collection.h>

class myApplication : public MonaApplication {

  public:
    myApplication(char* name);
    virtual ~myApplication() {}

  public:
    virtual int main();
    virtual void onKeyDown(int keycode, int modifiers);
};

myApplication::myApplication(char* name) : MonaApplication(name) {
}

int myApplication::main() {


    Screen screen;
    screen.fillRect16(10, 10, 40, 50, Color::rgb(105, 141, 148));

    dword pid = Message::lookup("SHELL.SVR");

    MemoryMap* mm = MemoryMap::create();

    mm->map(pid, 0x90000000, 0x90005000, 4096);

    strcpy((char*)0x90005000, "data share Mona");

    return 0;
}

void myApplication::onKeyDown(int keycode, int modifiers) {
    printf("down\n");
}

int main() {

    monaApp = new myApplication("HELLO.ELF");
    monaApp->main();

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

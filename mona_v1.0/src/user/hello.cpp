#include <userlib.h>
#include <string.h>
#include <collection.h>

int main() {

    printf("Hello World\n");

    dword mypid = Message::lookup("HELLO.ELF");
    dword pid   = Message::lookup("MAP.SVR");

    MessageInfo message;
    message.header = MSG_MAP;
    message.arg1   = mypid;
    message.arg2   = 0x20000000;
    message.from   = mypid;
    strcpy(message.str, "mona://device/fd/buffer");

    if (Message::send(pid, &message)) {
        printf("send error\n");
    }

    for (;;) {
        /* receive */
        if (!Message::receive(&message)) {

            switch(message.header) {

            case MSG_RESULT_OK:
                printf("contents=%s", (char*)0x20000000);
                break;

            default:
                /* igonore this message */
                break;
            }
        }
    }

    return 0;
}

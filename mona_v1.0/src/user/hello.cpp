#include <userlib.h>

int main() {
    sleep(100);
    print("Mona!!!_( ;'[]`)_");
    sleep(100);
    print("Mona!!!|( ;'<>`)/");

    char* p = (char*)umalloc(1024);
    *p = 0xFF;
    ufree(p);

//     for (int x = 10;  x < 150; x++) {
//         for (char color = 0; color < 16; color++) {
//             _put_pixel(x, 5 + color, color);
//         }
//     }


    print("");
    Message message;
    char buf[32];
    for (;;) {

        if (!_receive(&message)) {

        }
            buf[1] = '[';
            buf[0] = (char)message.arg1;
            buf[0] = ']';
            buf[1] = '\0';
            print(buf);

    }

    return 0;
}
